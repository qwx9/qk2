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
static int cpid = -1;
static cvar_t *ccdvol;

static int
cdinfo(void)
{
	int fd, i, n;
	Dir *d;
	Reprog *pat;

	ntrk = 0;

	if((fd = open("/mnt/cd", OREAD)) < 0)
		return -1;
	n = dirreadall(fd, &d);
	close(fd);
	if(n < 0)
		return -1;
	pat = regcomp("[au][0-9][0-9][0-9]");
	for(i = 0; i < n; i++)
		if(regexec(pat, d[i].name, nil, 0)){
			if(!trtype)
				trtype = d[i].name[0];
			ntrk++;
		}
	free(pat);
	free(d);
	if(ntrk < 1)
		return -1;
	return 0;
}

static void
cproc(void *)
{
	int a, n, afd, fd;
	char s[24];
	uchar buf[Nsbuf];
	short *p, *e;

	if((afd = open("/dev/audio", OWRITE)) < 0){
		fprint(2, "cd: open: %r\n");
		return;
	}
	fd = -1;
	for(;;){
		if(chtrk > 0){
			close(fd);
			trk = chtrk;
			snprint(s, sizeof s, "/mnt/cd/%c%03ud", trtype, trk);
			if((fd = open(s, OREAD)) < 0)
				fprint(2, "cd: open: %r\n");
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
		e = (short *)(buf + n);
		while(p < e){
			a = *p * cdvol;
			*p++ = a;
		}
		if(write(afd, buf, n) != n)
			break;
		/* threadkill requires that a libthread function be used
		 * somewhere: thread(2), errors notes and resources */
		yield();
	}
}

void
CDAudio_Play(int nt, qboolean loop)
{
	if(cpid < 0)
		return;
	nt -= 1;	/* d001 was assumed part of the track list */
	if(nt < 1 || nt > ntrk){
		Com_Printf("cd: invalid track number %d\n", nt);
		return;
	}

	chtrk = nt;
	cdloop = loop;
	cdvol = ccdvol->value;
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

	if(Cmd_Argc() < 2){
usage:
		Com_Printf("cd (play|loop|stop|pause|resume|info) [track]\n");
		return;
	}
	cmd = Cmd_Argv(1);
	if(cistrcmp(cmd, "play") == 0){
		if(Cmd_Argc() < 3)
			goto usage;
		CDAudio_Play(atoi(Cmd_Argv(2)), false);
	}else if(cistrcmp(cmd, "loop") == 0){
		if(Cmd_Argc() < 3)
			goto usage;
		CDAudio_Play(atoi(Cmd_Argv(2)), true);
	}else if(cistrcmp(cmd, "stop") == 0)
		CDAudio_Stop();
	else if(cistrcmp(cmd, "pause") == 0)
		CDAudio_Pause();
	else if(cistrcmp(cmd, "resume") == 0)
		CDAudio_Resume();
	else if(cistrcmp(cmd, "info") == 0)
		Com_Printf("track %d/%d; loop %d; vol %.1f\n", trk, ntrk, cdloop, cdvol);
	else
		goto usage;
}

void
CDAudio_Update(void)
{
	if(cpid < 0)
		return;
	if(ccdvol->value <= 0.0 || cdread == 0)
		return;
	cdvol = ccdvol->value;
	cdread = cdvol > 0.0;
}

int
CDAudio_Init(void)
{
	int tid;

	if(cdinfo() < 0)
		return -1;
	ccdvol = Cvar_Get("cdvol", "1", CVAR_ARCHIVE);
	if((tid = proccreate(cproc, nil, 16384)) < 0)
		sysfatal("proccreate: %r");
	cpid = threadpid(tid);
	Cmd_AddCommand("cd", CD_f);

	Com_Printf("CD Audio Initialized\n");
	return 0;
}

void
CDAudio_Shutdown(void)
{
	if(cpid < 0)
		return;

	CDAudio_Stop();
	if(postnote(PNPROC, cpid, "die die die") < 0)
		fprint(2, "CDAudio_Shutdown: postnote: %r\n");
	cpid = -1;
	cdread = cdloop = 0;
}
