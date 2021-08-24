#include <u.h>
#include <libc.h>
#include <stdio.h>
#include <ctype.h>
#include <thread.h>
#include "dat.h"
#include "fns.h"

void KBD_Update(void);

mainstacksize = 512*1024;
qboolean debug;
int curtime;
uint sys_frame_time;
Channel *fuckchan, *tchan;

static uchar *membase;
static int maxhunksize, curhunksize;
static char findbase[MAX_OSPATH], findpath[MAX_OSPATH], findpattern[MAX_OSPATH];
static Dir *dirs;
static long dirn, di;

static int glob_match(char *, char *);


/* Like glob_match, but match PATTERN against any final segment of TEXT.  */
static int
glob_match_after_star(char *pattern, char *text)
{
	char *p = pattern, *t = text;
	char c, c1;

	while ((c = *p++) == '?' || c == '*')
		if (c == '?' && *t++ == '\0')
			return 0;

	if (c == '\0')
		return 1;

	if (c == '\\')
		c1 = *p;
	else
		c1 = c;

	while (1) {
		if ((c == '[' || *t == c1) && glob_match(p - 1, t))
			return 1;
		if (*t++ == '\0')
			return 0;
	}
}

/* Return nonzero if PATTERN has any special globbing chars in it.  */
static int
glob_pattern_p(char *pattern)
{
	char *p = pattern;
	char c;
	int open = 0;

	while ((c = *p++) != '\0')
		switch (c) {
		case '?':
		case '*':
			return 1;

		case '[':		/* Only accept an open brace if there is a close */
			open++;		/* brace to match it.  Bracket expressions must be */
			continue;	/* complete, according to Posix.2 */
		case ']':
			if (open)
				return 1;
			continue;

		case '\\':
			if (*p++ == '\0')
				return 0;
		}

	return 0;
}

/* Match the pattern PATTERN against the string TEXT;
   return 1 if it matches, 0 otherwise.

   A match means the entire string TEXT is used up in matching.

   In the pattern string, `*' matches any sequence of characters,
   `?' matches any character, [SET] matches any character in the specified set,
   [!SET] matches any character not in the specified set.

   A set is composed of characters or ranges; a range looks like
   character hyphen character (as in 0-9 or A-Z).
   [0-9a-zA-Z_] is the set of characters allowed in C identifiers.
   Any other character in the pattern must be matched exactly.

   To suppress the special syntactic significance of any of `[]*?!-\',
   and match the character exactly, precede it with a `\'.
*/

static int
glob_match(char *pattern, char *text)
{
	char *p = pattern, *t = text;
	char c, c1, cstart, cend;
	int invert;

	while ((c = *p++) != '\0')
		switch (c) {
		case '?':
			if (*t == '\0')
				return 0;
			else
				++t;
			break;

		case '\\':
			if (*p++ != *t++)
				return 0;
			break;

		case '*':
			return glob_match_after_star(p, t);

		case '[':
			{
				c1 = *t++;

				if (!c1)
					return (0);

				invert = ((*p == '!') || (*p == '^'));
				if (invert)
					p++;

				c = *p++;
				while (1) {
					cstart = c;
					cend = c;

					if (c == '\\') {
						cstart = *p++;
						cend = cstart;
					}
					if (c == '\0')
						return 0;

					c = *p++;
					if (c == '-' && *p != ']') {
						cend = *p++;
						if (cend == '\\')
							cend = *p++;
						if (cend == '\0')
							return 0;
						c = *p++;
					}
					if (c1 >= cstart && c1 <= cend)
						goto match;
					if (c == ']')
						break;
				}
				if (!invert)
					return 0;
				break;

			  match:
				/* Skip the rest of the [...] construct that already matched.  */
				while (c != ']') {
					if (c == '\0')
						return 0;
					c = *p++;
					if (c == '\0')
						return 0;
					else if (c == '\\')
						++p;
				}
				if (invert)
					return 0;
				break;
			}

		default:
			if (c != *t++)
				return 0;
		}

	/* if the pattern is empty, Sys_FindNext looks at the current file anyway */
	return *t == '\0';
}

void *
Hunk_Begin(int maxsize)
{
	// reserve a huge chunk of memory, but don't commit any yet
	maxhunksize = maxsize;
	curhunksize = 0;
	if((membase = mallocz(maxhunksize, 1)) == nil)
		sysfatal("Hunk_Begin:malloc %d: %r", maxhunksize);
	return membase;
}

void *
Hunk_Alloc(int size)
{
	byte *buf;

	// round to cacheline
	size = (size+31)&~31;
	if(curhunksize + size > maxhunksize)
		Sys_Error("Hunk_Alloc overflow");
	buf = membase + curhunksize;
	curhunksize += size;
	return buf;
}

int
Hunk_End(void)
{
	if(realloc(membase, curhunksize) != membase)
		sysfatal("Hunk_End:realloc: %r");
	return curhunksize;
}

void
Hunk_Free(void *base)
{
	if(base != nil)
		free(base);
}

char *
Sys_FindFirst(char *path, int f)
{
	char *p;
	int fd;

	if(dirs != nil)
		Sys_Error("Sys_BeginFind without close");

	strncpy(findbase, path, sizeof findbase-1);
	if((p = strrchr(findbase, '/')) != nil){
		*p = 0;
		strncpy(findpattern, p+1, sizeof findpattern-1);
	}else
		strcpy(findpattern, "*");
	if(strcmp(findpattern, "*.*") == 0)
		strcpy(findpattern, "*");
	if(*findpattern == '\0'){
		Com_Printf("Sys_BeginFind: empty pattern\n");
		return nil;
	}

	if((fd = open(findbase, OREAD)) < 0){
		fprint(2, "Sys_BeginFind:open: %r\n");
		return nil;
	}
	dirn = dirreadall(fd, &dirs);
	close(fd);
	if(dirn == 0)
		return nil;
	if(dirn < 0){
		fprint(2, "Sys_BeginFind:dirread: %r\n");
		return nil;
	}

	di = 0;
	return Sys_FindNext(f);
}

/* if f is DMDIR, only retain directories; otherwise always exclude them */
char *
Sys_FindNext(int f)
{
	int i;

	if(dirs == nil)
		Sys_Error("Sys_FindNext without open\n");
	while(di < dirn){
		i = di++;
		if(!(f ^ dirs[i].mode & DMDIR) && glob_match(findpattern, dirs[i].name)){
			snprint(findpath, sizeof findpath, "%s/%s", findbase, dirs[i].name);
			return findpath;
		}
	}
	return nil;
}

void
Sys_FindClose(void)
{
	if(dirs != nil){
		free(dirs);
		dirs = nil;
	}
}

/* prints to "debugging console" */
void
Sys_ConsoleOutput(char *s)
{
	if(!debug)
		return;
	fprint(2, "%s", s);
}

void
Sys_Error(char *error, ...)
{ 
	char buf[1024];
	va_list arg;

	CL_Shutdown();

	va_start(arg, error);
	vseprint(buf, buf+sizeof(buf), error, arg);
	va_end(arg);
	fprint(2, "%s\n", buf);
	sysfatal("ending.");
}

int
Sys_Milliseconds(void)
{
	static long msbase;

	if(msbase == 0)
		msbase = time(nil)*1000;
	curtime = nsec()/1000000 - msbase;
	return curtime;
}

vlong
flen(int fd)
{
	uchar bs[1024];

	if(fstat(fd, bs, sizeof bs) < 0){
		fprint(2, "flen:fstat: %r\n");
		return -1;
	}
	return *((vlong *)(bs+2+2+4+1+4+8+4+4+4));	/* length[8] */
}

void
Sys_SendKeyEvents(void)
{
	KBD_Update();
	sys_frame_time = Sys_Milliseconds();	// grab frame time 
}

char *
Sys_GetClipboardData(void)
{
	return nil;
}

void
Sys_Quit(void)
{
	chanfree(fuckchan);
	chanfree(tchan);
	threadexitsall(nil);
}

void
Sys_Init(void)
{
	//Sys_SetFPCW();
	if((fuckchan = chancreate(sizeof(int), 1)) == nil)
		sysfatal("chancreate fuckchan: %r");
	if((tchan = chancreate(sizeof(int), 16)) == nil)
		sysfatal("chancreate tchan: %r");
}

void *
emalloc(ulong n)
{
	void *p;

	if(p = mallocz(n, 1), p == nil)
		sysfatal("emalloc %r");
	setmalloctag(p, getcallerpc(&n));
	return p;
}

/* threadnotify does not allow handling sys: notes:
 * /sys/src/libthread/note.c:93; most important is releasing the
 * mouse, the rest is essentially killing all children as well
 * as the parent (in case one of the other procs crashed);
 * broken procs will remain afterwards, but not if threadexits
 * et al is called.  for threadkill to work, the proc must be
 * using a libthread function (at least yield), otherwise it
 * won't do diddly.  too much could go wrong if going through 
 * all the shutdown functions. */
void
croak(void *, char *note)
{
	if(!strncmp(note, "sys:", 4)){
		IN_Grabm(0);
		threadkillgrp(THnet);
		threadkillgrp(0);
	}
	noted(NDFLT);
}

void
threadmain(int argc, char *argv[])
{
	int time, oldtime, newtime;

	setfcr(getfcr() & ~(FPOVFL|FPUNFL|FPINVAL|FPZDIV));	/* assumed ignored in code */
	notify(croak);

	srand(getpid());
	Qcommon_Init(argc, argv);

	oldtime = Sys_Milliseconds();
	for(;;){
		do{
			newtime = Sys_Milliseconds();
			time = newtime - oldtime;
		}while(time < 1);	// find time spent rendering last frame
		Qcommon_Frame(time);
		oldtime = newtime;
	}
}
