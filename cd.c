#include <u.h>
#include <libc.h>
#include <stdio.h>
#include <thread.h>
#include <regexp.h>
#include "dat.h"
#include "fns.h"

static int cdread, cdloop;
static double cdvol;
static int ntrk, trk, chtrk;
static char trtype;
static int ctid = -1;
static Reprog *pat;
static cvar_t *ccdvol;

static int
cdinfo(void)
{
	int fd, i, n;
	Dir *d;

	ntrk = 0;

	if((fd = open("/mnt/cd", OREAD)) < 0)
		goto err;
	if((n = dirreadall(fd, &d)) < 0)
		goto err;
	close(fd);
	for(i = 0; i < n; i++)
		if(regexec(pat, d[i].name, nil, 0)){
			if(!trtype)
				trtype = d[i].name[0];
			ntrk++;
		}
	free(d);
	if(ntrk < 1)
		return -1;
	return 0;

err:
	close(fd);
	fprint(2, "cdinfo: %r\n");
	return -1;
}

static void
cproc(void *)
{
	int a, n, afd, fd;
	char s[24];
	uchar buf[Nsbuf];
	short *p, *e;

	if((afd = open("/dev/audio", OWRITE)) < 0)
		return;
	fd = -1;
	for(;;){
		if(chtrk > 0){
			close(fd);
			trk = chtrk;
			snprint(s, sizeof s, "/mnt/cd/%c%03ud", trtype, trk);
			if((fd = open(s, OREAD)) < 0)
				fprint(2, "cproc: %r");
			chtrk = 0;
		}
		if(!cdread || fd < 0){
			sleep(1);
			continue;
		}
		if((n = read(fd, buf, sizeof buf)) < 0)
			break;
		if(n == 0){
			if(cdloop)
				seek(fd, 0, 0);
			else{
				close(fd);
				fd = -1;
			}
			continue;
		}
		p = (short *)buf;
		e = (short *)(buf+sizeof buf);
		while(p < e){
			a = *p * cdvol;
			*p++ = a;
		}
		if(write(afd, buf, n) != n)
			break;
	}
}

void
CDAudio_Play(int nt, qboolean loop)
{
	if(ctid < 0)
		return;
	nt -= 1;	/* d001 was assumed part of the track list */
	if(nt < 1 || nt > ntrk){
		Com_Printf("cd: invalid track number %d\n", nt);
		return;
	}

	chtrk = nt;
	cdloop = loop;
	if(cdvol > 0)
		cdread = 1;
}

void
CDAudio_Stop(void)
{
	cdread = 0;
	cdloop = 0;
}

void
CDAudio_Pause(void)
{
	cdread = 0;
}

void
CDAudio_Resume(void)
{
	cdread = 1;
}

void
CD_f(void)
{
	char *cmd;

	if(Cmd_Argc() < 2)
		return;
	cmd = Cmd_Argv(1);
	if(cistrcmp(cmd, "play") == 0){
		CDAudio_Play(atoi(Cmd_Argv(2)), false);
		return;
	}else if(cistrcmp(cmd, "loop") == 0){
		CDAudio_Play(atoi(Cmd_Argv(2)), true);
		return;
	}else if(cistrcmp(cmd, "stop") == 0){
		CDAudio_Stop();
		return;
	}else if(cistrcmp(cmd, "pause") == 0){
		CDAudio_Pause();
		return;
	}else if(cistrcmp(cmd, "resume") == 0){
		CDAudio_Resume();
		return;
	}else if(cistrcmp(cmd, "info") == 0){
		Com_Printf("track %d/%d; loop %d; vol %.1f\n", trk, ntrk, cdloop, cdvol);
		return;
	}
}

void
CDAudio_Update(void)
{
	if(ctid < 0)
		return;
	cdvol = ccdvol->value;
	cdread = cdvol > 0.0;
}

int
CDAudio_Init(void)
{
	pat = regcomp("[au][0-9][0-9][0-9]");
	if(cdinfo() < 0)
		return -1;
	ccdvol = Cvar_Get("cdvol", "1", CVAR_ARCHIVE);
	if((ctid = proccreate(cproc, nil, 16384)) < 0)
		sysfatal("proccreate: %r");
	Cmd_AddCommand("cd", CD_f);

	Com_Printf("CD Audio Initialized\n");
	return 0;
}

void
CDAudio_Shutdown(void)
{
	if(ctid < 0)
		return;

	CDAudio_Stop();
	postnote(PNPROC, ctid, "shutdown");
	ctid = -1;
	free(pat);
	pat = nil;
	cdread = cdloop = 0;
}
