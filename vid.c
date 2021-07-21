#include <u.h>
#include <libc.h>
#include <draw.h>
#include <stdio.h>
#include <ctype.h>
#include "dat.h"
#include "fns.h"

refexport_t re;	/* exported functions from refresh DLL */
int resized, dumpwin;
Point center;
Rectangle grabr;

typedef ulong PIXEL;

static int rwon;
static uchar *fb, *fbs;
static Image *fbi;
static s32int fbpal[256];
static int scaleon, scale = 1;
static Rectangle fbr;

refexport_t GetRefAPI(refimport_t);


static void
scalefb(void)
{
	int dy, *p, c, *s;

	if(scale < 2)
		return;
	p = (s32int *)fbs;
	s = (s32int *)fb;
	dy = vid.width * vid.height;
	while(dy-- > 0){
		c = *s++;
		switch(scale){
		case 16: p[15] = c;
		case 15: p[14] = c;
		case 14: p[13] = c;
		case 13: p[12] = c;
		case 12: p[11] = c;
		case 11: p[10] = c;
		case 10: p[9] = c;
		case 9: p[8] = c;
		case 8: p[7] = c;
		case 7: p[6] = c;
		case 6: p[5] = c;
		case 5: p[4] = c;
		case 4: p[3] = c;
		case 3: p[2] = c;
		case 2: p[1] = c; p[0] = c;
		}
		p += scale;
	}
}

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

/* only exists to allow taking tear-free screenshots ingame... */
static int
writebit(void)
{
	int n, fd;
	char *s;

	mkdir(va("%s/scrnshot", ri.FS_Gamedir()));
	s = va("%s/scrnshot/quake.%d.bit", ri.FS_Gamedir(), time(nil));
	if(access(s, AEXIST) != -1){
		fprint(2, "writebit: not overwriting %s\n", s);
		return -1;
	}
	if(fd = create(s, OWRITE, 0644), fd < 0)
		return -1;
	n = writeimage(fd, fbi, 0);
	close(fd);
	if(n >= 0)
		ri.Con_Printf(PRINT_ALL, "Wrote %s\n", s);
	return n;
}

static void
resetfb(void)
{
	Point p;

	if(scaleon){
		scale = Dx(screen->r) / vid.width;
		if(scale <= 0)
			scale = 1;
		else if(scale > 16)
			scale = 16;
	}else{
		vid.width = Dx(screen->r);
		vid.height = Dy(screen->r);
	}
	vid.rowbytes = vid.width * sizeof *fbpal;
	center = divpt(addpt(screen->r.min, screen->r.max), 2);
	p = Pt(scale * vid.width/2, scale * vid.height/2);
	fbr = Rpt(subpt(center, p), addpt(center, p));
	p = Pt(vid.width/4, vid.height/4);
	grabr = Rpt(subpt(center, p), addpt(center, p));
	free(fb);
	freeimage(fbi);
	if((fbi = allocimage(display,
		Rect(0, 0, vid.width * scale, scale > 1 ? 1 : vid.height),
		XRGB32, scale > 1, 0)) == nil)
		sysfatal("resetfb: %r");
	fb = emalloc(vid.rowbytes * vid.height);
	if(scaleon){
		free(fbs);
		fbs = emalloc(vid.rowbytes * scale * vid.height);
	}
	vid.buffer = fb;
	draw(screen, screen->r, display->black, nil, ZP);
	sw_mode->modified = true;	/* make ref_soft refresh its shit */
}

void
flipfb(void)
{
	uchar *p;
	Rectangle r;

	if(resized){
		resized = 0;
		if(getwindow(display, Refnone) < 0)
			sysfatal("getwindow: %r\n");
		resetfb();
		return;
	}
	drawfb();
	scalefb();
	if(scale == 1){
		loadimage(fbi, Rect(0,0,vid.width,vid.height), fb, vid.height * vid.rowbytes);
		draw(screen, Rpt(fbr.min, Pt(fbr.max.x, fbr.max.y)), fbi, nil, ZP);
	}else{
		p = fbs;
		r = fbr;
		while(r.min.y < fbr.max.y){
			r.max.y = r.min.y + scale;
			p += loadimage(fbi, fbi->r, p, vid.rowbytes * scale);
			draw(screen, r, fbi, nil, ZP);
			r.min.y = r.max.y;
		}
	}
	flushimage(display, 1);
	if(dumpwin){
		if(writebit() < 0)
			ri.Con_Printf(PRINT_ALL, "Could not write screenshot\n");
		dumpwin = 0;
	}
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

static void
setscale(void)
{
	char *s, *p;
	static cvar_t *scale;

	scale = Cvar_Get("scale", "", 0);
	if(strlen(scale->string) < 3+1+3)
		return;
	s = scale->string;
	vid.width = strtol(s, &p, 10);
	if(p == s || vid.width < 320){
		fprint(2, "setscale: invalid width %d\n", vid.width);
		return;
	}
	if(*p++ != 'x'){
		fprint(2, "setscale: invalid resolution\n");
		return;
	}
	vid.height = strtol(p, &s, 10);
	if(p == s || vid.height < 200){
		fprint(2, "setscale: invalid height %d\n", vid.height);
		return;
	}
	scaleon = 1;
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
	setscale();
	re.Init();
	resetfb();
}
