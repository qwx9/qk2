#include <u.h>
#include <libc.h>
#include <stdio.h>
#include <thread.h>
#include "../dat.h"
#include "../fns.h"

int sndon;

enum{
	Nbuf	= 32
};
int audio_fd;
int wpos;
int stid = -1;
Channel *schan;

cvar_t *sndbits;
cvar_t *sndspeed;
cvar_t *sndchannels;
cvar_t *snddevice;
cvar_t *nosound;


void sproc (void *)
{
	int n;

	for(;;){
		if(recv(schan, nil) < 0){
			fprint(2, "sproc:recv %r\n");
			break;
		}
		if((n = write(audio_fd, dma.buffer, dma.samplebits/8 * dma.samples)) < 0){
			fprint(2, "sproc:write %r\n");
			break;
		}
		wpos += n;
	}
	stid = -1;
}

qboolean SNDDMA_Init(void)
{
	if(sndon)
		return false;

	if(COM_CheckParm("-nosound"))	/* FIXME */
		return false;

	if(!snddevice){
		sndbits = Cvar_Get("sndbits", "16", CVAR_ARCHIVE);
		sndspeed = Cvar_Get("sndspeed", "44100", CVAR_ARCHIVE);
		sndchannels = Cvar_Get("sndchannels", "2", CVAR_ARCHIVE);
		snddevice = Cvar_Get("snddevice", "/dev/audio", CVAR_ARCHIVE);
	}

	if((audio_fd = open(snddevice->string, OWRITE)) < 0){
		fprint(2, "SNDDMA_Init:open %r\n");
		return false;
	}

	dma.samplebits = (int)sndbits->value;
	if(dma.samplebits != 16 && dma.samplebits != 8)
		dma.samplebits = 16;
	dma.speed = (int)sndspeed->value;
	if(dma.speed != 44100)
		dma.speed = 44100;
	dma.channels = (int)sndchannels->value;
	if(dma.channels < 1 || dma.channels > 2)
		dma.channels = 2;
	dma.samples = 8192;
	dma.submission_chunk = 1;
	if((dma.buffer = mallocz(dma.samplebits/8 * dma.samples, 1)) == nil)
		sysfatal("SNDDMA_Init:mallocz: %r\n");
	dma.samplepos = 0;
	sndon = 1;

	schan = chancreate(sizeof(int), Nbuf);
	if((stid = proccreate(sproc, nil, 8192)) < 0){
		stid = -1;
		SNDDMA_Shutdown();
		sysfatal("SNDDMA_Init:proccreate: %r\n");
	}
	return true;
}

int SNDDMA_GetDMAPos(void)
{
	if(!sndon)
		return 0;
	dma.samplepos = wpos / (dma.samplebits/8);
	return dma.samplepos;
}

void SNDDMA_Shutdown(void)
{
	if(!sndon)
		return;
	if(stid != -1){
		threadkill(stid);
		stid = -1;
	}
	if(schan != nil){
		chanfree(schan);
		schan = nil;
	}
	free(dma.buffer);
	close(audio_fd);
	sndon = 0;
}

void SNDDMA_Submit(void)
{
	if(nbsend(schan, nil) < 0){
		fprint(2, "SNDDMA_Submit:nbsend: %r\n");
		SNDDMA_Shutdown();
	}
}

void SNDDMA_BeginPainting (void)
{
}
