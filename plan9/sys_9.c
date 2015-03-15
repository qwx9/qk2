#include <u.h>
#include <libc.h>
#include <stdio.h>
#include <ctype.h>
#include <thread.h>
#include "../dat.h"
#include "../fns.h"

mainstacksize = 512*1024;	/* FIXME */

uint sys_frame_time;
qboolean stdin_active = true;

void KBD_Update(void);


/* prints to "debugging console" */
void Sys_ConsoleOutput (char *string)
{
	write(1, string, strlen(string));
}

void Sys_Quit (void)
{
	CL_Shutdown();
	Qcommon_Shutdown();
	threadexitsall(nil);
}

void Sys_Init(void)
{
	//Sys_SetFPCW();
}

void Sys_Error (char *error, ...)
{ 
	char buf[1024], *out;
	va_list arg;

	CL_Shutdown();
	Qcommon_Shutdown();

	va_start(arg, error);
	out = vseprint(buf, buf+sizeof(buf), error, arg);
	va_end(arg);
	write(2, buf, out-buf);
	print("\n");
	sysfatal("ending.");
}

int Sys_FileTime (char *path)
{
	uchar sb[1024];

	if(stat(path, sb, sizeof sb) < 0){
		fprint(2, "Sys_FileTime:stat: %r\n");
		return -1;
	}
	return *((int *)(sb+25));
}

char *Sys_ConsoleInput(void)
{
	static char text[256];
	int n;

	if(!dedicated || !dedicated->value || !stdin_active)
		return nil;

	if((n = read(0, text, sizeof(text))) < 0)
		return nil;
	if(n == 0){
		stdin_active = false;
		return nil;
	}
	text[n-1] = '\0';	/* rip off \n */
	return text;
}

void Sys_UnloadGame (void)
{
}

void Sys_AppActivate (void)
{
}

void Sys_SendKeyEvents (void)
{
#ifndef DEDICATED_ONLY
	KBD_Update();
#endif
	sys_frame_time = Sys_Milliseconds();	// grab frame time 
}

char *Sys_GetClipboardData (void)
{
	return nil;
}

void Sys_CopyProtect (void)
{
}

void croak (void *, char *note)
{
	if(!strncmp(note, "sys:", 4)){
		IN_Shutdown();
		SNDDMA_Shutdown();
	}
	noted(NDFLT);
}

void threadmain (int argc, char *argv[])
{
	int time, oldtime, newtime;

	setfcr(getfcr() & ~(FPOVFL|FPUNFL|FPINVAL|FPZDIV));	/* assumed ignored in code */
	notify(croak);

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
