#include <u.h>
#include <libc.h>
#include <stdio.h>
#include <ctype.h>
#include <thread.h>
#include "../q_shared.h"

mainstacksize = 512*1024;
uint sys_frame_time;
Channel *fuckchan, *tchan;

void KBD_Update(void);


/* prints to "debugging console" */
void
Sys_ConsoleOutput(char *s)
{
	write(1, s, strlen(s));
}

void
Sys_Error(char *error, ...)
{ 
	char buf[1024], *out;
	va_list arg;

	CL_Shutdown();

	va_start(arg, error);
	out = vseprint(buf, buf+sizeof(buf), error, arg);
	va_end(arg);
	write(2, buf, out-buf);
	print("\n");
	sysfatal("ending.");
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

int
Sys_FileTime(char *path)
{
	uchar sb[1024];

	if(stat(path, sb, sizeof sb) < 0){
		fprint(2, "Sys_FileTime:stat: %r\n");
		return -1;
	}
	return *((int *)(sb+25));
}

void
Sys_UnloadGame(void)
{
}

void
Sys_AppActivate(void)
{
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
Sys_CopyProtect(void)
{
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

void
croak(void *, char *note)
{
	if(!strncmp(note, "sys:", 4)){
		IN_Shutdown();
		SNDDMA_Shutdown();
		NET_Shutdown();
	}
	noted(NDFLT);
}

void
threadmain(int argc, char *argv[])
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
