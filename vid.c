#include <u.h>
#include <libc.h>
#include <draw.h>
#include <stdio.h>
#include <ctype.h>
#include "dat.h"
#include "fns.h"

refexport_t re;	/* exported functions from refresh DLL */
int resized;
Point center;
Rectangle grabr;

typedef ulong PIXEL;

static int rwon;
static uchar *fb;
static Image *fbi;
static s32int fbpal[256];

refexport_t GetRefAPI(refimport_t);


static void
drawfb(void)
{
	uchar *s;
	int n, we, w8, wr, *d, dy;

	we = vid.width - 1;
	w8 = vid.width + 7 >> 3;
	wr = vid.width % 8;
	dy = vid.height * vid.rowbytes;
	while((dy -= vid.rowbytes) >= 0){
		s = fb + dy;
		d = ((int *)s) + we;
		s += we;
		n = w8;
		switch(wr){
		case 0:	do{	*d-- = fbpal[*s--];
		case 7:		*d-- = fbpal[*s--];
		case 6:		*d-- = fbpal[*s--];
		case 5:		*d-- = fbpal[*s--];
		case 4:		*d-- = fbpal[*s--];
		case 3:		*d-- = fbpal[*s--];
		case 2:		*d-- = fbpal[*s--];
		case 1:		*d-- = fbpal[*s--];
			}while(--n > 0);
		}
	}
}

static void
resetfb(void)
{
	Point p;

	vid.width = Dx(screen->r);
	vid.height = Dy(screen->r);
	free(fb);
	fb = emalloc(vid.width * vid.height * sizeof *fbpal);
	freeimage(fbi);
	if((fbi = allocimage(display, Rect(0, 0, vid.width, vid.height), XRGB32, 0, 0)) == nil)
		sysfatal("allocimage: %r");
	vid.buffer = fb;
	vid.rowbytes = vid.width * sizeof *fbpal;
	center = addpt(screen->r.min, Pt(vid.width/2, vid.height/2));
	p = Pt(vid.width/4, vid.height/4);
	grabr = Rpt(subpt(center, p), addpt(center, p));
	sw_mode->modified = true;	/* make ref_soft refresh its shit */
}

void
flipfb(void)
{
	if(resized){
		resized = 0;
		if(getwindow(display, Refnone) < 0)
			sysfatal("getwindow: %r\n");
		resetfb();
		return;
	}
	drawfb();
	loadimage(fbi, fbi->r, fb, vid.height * vid.rowbytes);
	draw(screen, screen->r, fbi, nil, ZP);
	flushimage(display, 1);
}

void
setpal(uchar *p)
{
	int *fp;

	if(p == nil)
        	p = (uchar *)sw_state.currentpalette;
	for(fp=fbpal; fp<fbpal+nelem(fbpal); p+=4)
		*fp++ = p[0] << 16 | p[1] << 8 | p[2];
}

void
VID_Printf(int print_level, char *fmt, ...)
{
	va_list argptr;
	char msg[4096];
	
	va_start(argptr, fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);
	if(print_level == PRINT_ALL)
		Com_Printf("%s", msg);
	else
		Com_DPrintf("%s", msg);
}

void
VID_Error(int err_level, char *fmt, ...)
{
	va_list argptr;
	char msg[4096];
	
	va_start(argptr, fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);
	Com_Error(err_level, "%s", msg);
}

void
initfb(void)
{
	refimport_t ri;

	if(initdraw(nil, nil, "quake2") < 0)
		sysfatal("VID_Init:initdraw: %r\n");
	vidref_val = VIDREF_SOFT;
	ri.Cmd_AddCommand = Cmd_AddCommand;
	ri.Cmd_RemoveCommand = Cmd_RemoveCommand;
	ri.Cmd_Argc = Cmd_Argc;
	ri.Cmd_Argv = Cmd_Argv;
	ri.Cmd_ExecuteText = Cbuf_ExecuteText;
	ri.Con_Printf = VID_Printf;
	ri.Sys_Error = VID_Error;
	ri.FS_LoadFile = FS_LoadFile;
	ri.FS_FreeFile = FS_FreeFile;
	ri.FS_Gamedir = FS_Gamedir;
	ri.Cvar_Get = Cvar_Get;
	ri.Cvar_Set = Cvar_Set;
	ri.Cvar_SetValue = Cvar_SetValue;
	ri.Vid_MenuInit = VID_MenuInit;

	re = GetRefAPI(ri);
	re.Init();
	resetfb();
}
