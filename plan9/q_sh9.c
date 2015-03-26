#include <u.h>
#include <libc.h>
#include <stdio.h>
#include "../q_shared.h"

byte *membase;
int maxhunksize;
int curhunksize;
int curtime;
char findbase[MAX_OSPATH], findpath[MAX_OSPATH], findpattern[MAX_OSPATH];
int fdir = -1;
Dir *ddir;

int	glob_match(char *, char *);


/* Like glob_match, but match PATTERN against any final segment of TEXT.  */
int glob_match_after_star(char *pattern, char *text)
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
int glob_pattern_p(char *pattern)
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

int glob_match(char *pattern, char *text)
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

	return *t == '\0';
}

void *Hunk_Begin (int maxsize)
{
	// reserve a huge chunk of memory, but don't commit any yet
	maxhunksize = maxsize;
	curhunksize = 0;
	if((membase = mallocz(maxhunksize, 1)) == nil)
		sysfatal("Hunk_Begin:malloc %d: %r", maxhunksize);
	return membase;
}

void *Hunk_Alloc (int size)
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

int Hunk_End (void)
{
	if(realloc(membase, curhunksize) != membase)
		sysfatal("Hunk_End:realloc: %r");
	return curhunksize;
}

void Hunk_Free (void *base)
{
	if(base != nil)
		free(base);
}

int Sys_Milliseconds (void)
{
	static long msbase;

	if(msbase == 0)
		msbase = time(nil)*1000;
	curtime = nsec()/1000000 - msbase;
	return curtime;
}

void Sys_Mkdir (char *path)
{
	int d;

	if((d = create(path, OREAD, DMDIR|0777)) < 0)
		fprint(2, "Sys_mkdir:create: %r\n");
	else
		close(d);
}

qboolean CompareAttributes (Dir *d, uint musthave, uint canthave)
{
	if(d->mode & DMDIR && canthave & SFF_SUBDIR)
		return false;
	if(musthave & SFF_SUBDIR && ~d->mode & DMDIR)
		return false;
	return true;
}

char *Sys_FindFirst (char *path, uint musthave, uint canhave)
{
	char *p;
	long n;

	if(fdir != -1)
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

	if((fdir = open(findbase, OREAD)) < 0){
		fprint(2, "Sys_BeginFind:open: %r\n");
		return nil;
	}

	while((n = dirread(fdir, &ddir)) > 0){
		if(glob_match(findpattern, ddir->name)){
			if(CompareAttributes(ddir, musthave, canhave)){
				sprintf(findpath, "%s/%s", findbase, ddir->name);
				return findpath;
			}
		}
	}
	if(n < 0)
		fprint(2, "Sys_BeginFind:dirread: %r\n");
	return nil;
}

char *Sys_FindNext (uint musthave, uint canhave)
{
	long n;

	if(fdir == -1){
		Sys_Error("Sys_FindNext without open\n");
		return nil;
	}
	while((n = dirread(fdir, &ddir)) > 0){
		if(glob_match(findpattern, ddir->name)){
			if(CompareAttributes(ddir, musthave, canhave)){
				sprintf(findpath, "%s/%s", findbase, ddir->name);
				return findpath;
			}
		}
	}
	if(n < 0)
		fprint(2, "Sys_BeginFind:dirread: %r\n");
	return nil;
}

void Sys_FindClose (void)
{
	if(fdir != -1){
		close(fdir);
		free(ddir);
		fdir = -1;
	}
}
