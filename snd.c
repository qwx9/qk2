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

static int afd = -1, wpos;
enum{
	Nbuf = 4,
};
static Channel *schan;
static QLock sndlock;

static void
sproc(void *)
{
	int n;

	for(;;){
		if(recv(schan, nil) < 0)
			break;
		if((n = write(afd, dma.buffer, Nsbuf)) != Nsbuf)
			break;
		qlock(&sndlock);
		wpos += n;
		qunlock(&sndlock);
	}
}

int
SNDDMA_GetDMAPos(void)
{
	if(afd < 0)
		return 0;
	qlock(&sndlock);
	dma.samplepos = wpos / Sampsz;
	qunlock(&sndlock);
	return dma.samplepos;
}

void
SNDDMA_Submit(void)
{
	if(nbsend(schan, nil) < 0){
		fprint(2, "SNDDMA_Submit: %r\n");
		shutsnd();
	}
}

void
SNDDMA_BeginPainting(void)
{
}

void
shutsnd(void)
{
	if(afd < 0)
		return;
	afd = -1;
	close(afd);
	chanfree(schan);
	sound_started = 0;
	num_sfx = 0;
}

void
restartsnd(void)
{
	sfx_t *s;

	if(afd < 0)
		return;
	for(s=known_sfx+num_sfx; s<known_sfx+num_sfx*2; s++){
		if(!s->name[0])
			continue;
		if(s->cache)
			Z_Free(s->cache);
		memset(s, 0, sizeof*s);
	}
	num_sfx = 0;
	soundtime = 0;
	paintedtime = 0;
	S_StopAllSounds();
	CL_RegisterSounds();
}

void
initsnd(void)
{
	if((afd = open("/dev/audio", OWRITE)) < 0){
		fprint(2, "initsnd %r\n");
		return;
	}
	dma.samplebits = Sampsz * 8;
	dma.speed = Rate;
	dma.channels = 2;
	dma.samples = Nsamp;
	dma.submission_chunk = 1;
	dma.buffer = emalloc(Nsamp * Sampsz);
	dma.samplepos = 0;
	wpos = 0;
	schan = chancreate(sizeof(int), Nbuf);
	if(proccreate(sproc, nil, 8192) < 0)
		sysfatal("proccreate: %r\n");
	s_volume = Cvar_Get("s_volume", "0.7", CVAR_ARCHIVE);
	s_khz = Cvar_Get("s_khz", "22", CVAR_ARCHIVE);
	s_loadas8bit = Cvar_Get("s_loadas8bit", "0", CVAR_ARCHIVE);
	s_mixahead = Cvar_Get("s_mixahead", "0.2", CVAR_ARCHIVE);
	S_InitScaletable();
	sound_started = 1;
	num_sfx = 0;
	soundtime = 0;
	paintedtime = 0;
	S_StopAllSounds();
}
