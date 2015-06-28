#include <u.h>
#include <libc.h>
#include <stdio.h>
#include <thread.h>
#include "dat.h"
#include "fns.h"

static cvar_t *sndbits;
static cvar_t *sndspeed;
static cvar_t *sndchannels;
static cvar_t *snddev;

static int afd, sndon, wpos;
enum{
	Nbuf	= 32
};
static Channel *schan;
static QLock sndlock;


static void
sproc(void *)
{
	int n;

	threadsetgrp(THsnd);

	for(;;){
		if(recv(schan, nil) < 0){
			fprint(2, "sproc:recv %r\n");
			break;
		}
		if((n = write(afd, dma.buffer, dma.samplebits/8 * dma.samples)) < 0){
			fprint(2, "sproc:write %r\n");
			break;
		}
		qlock(&sndlock);
		wpos += n;
		qunlock(&sndlock);
	}
	fprint(2, "sproc %d: %r\n", threadpid(threadid()));
}

qboolean
SNDDMA_Init(void)
{
	if(sndon)
		return false;

	if(COM_CheckParm("-nosound"))
		return false;

	if(snddev == nil){
		sndbits = Cvar_Get("sndbits", "16", CVAR_ARCHIVE);
		sndspeed = Cvar_Get("sndspeed", "44100", CVAR_ARCHIVE);
		sndchannels = Cvar_Get("sndchannels", "2", CVAR_ARCHIVE);
		snddev = Cvar_Get("snddev", "/dev/audio", CVAR_ARCHIVE);
	}

	if((afd = open(snddev->string, OWRITE)) < 0){
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
	wpos = 0;

	schan = chancreate(sizeof(int), Nbuf);
	if(proccreate(sproc, nil, 8192) < 0){
		SNDDMA_Shutdown();
		sysfatal("SNDDMA_Init:proccreate: %r\n");
	}
	return true;
}

int
SNDDMA_GetDMAPos(void)
{
	if(!sndon)
		return 0;
	qlock(&sndlock);
	dma.samplepos = wpos / (dma.samplebits/8);
	qunlock(&sndlock);
	return dma.samplepos;
}

void
SNDDMA_Shutdown(void)
{
	if(!sndon)
		return;

	threadkillgrp(THsnd);
	close(afd);
	if(schan != nil){
		chanfree(schan);
		schan = nil;
	}
	free(dma.buffer);
	sndon = 0;
}

void
SNDDMA_Submit(void)
{
	if(nbsend(schan, nil) < 0){
		fprint(2, "SNDDMA_Submit:nbsend: %r\n");
		SNDDMA_Shutdown();
	}
}

void
SNDDMA_BeginPainting(void)
{
}
