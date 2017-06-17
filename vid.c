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

typedef ulong PIXEL;

static int rwon;
static uchar *framebuf;
static Image *fbim;
static PIXEL st2d_8to24table[256];
static int shiftmask_fl;
static int r_shift, g_shift, b_shift;
static uint r_mask, g_mask, b_mask;

refexport_t GetRefAPI(refimport_t);


static void
shiftmask_init(void)
{
	uint x;

	r_mask = 0xff0000;
	g_mask = 0xff00;
	b_mask = 0xff;
	for(r_shift = -8, x = 1; x < r_mask; x <<= 1)
		r_shift++;
	for(g_shift = -8, x = 1; x < g_mask; x <<= 1)
		g_shift++;
	for(b_shift = -8, x = 1; x < b_mask; x <<= 1)
		b_shift++;
	shiftmask_fl = 1;
}

static PIXEL
rgb24(int r, int g, int b)
{
	PIXEL p = 0;

	if(shiftmask_fl == 0)
		shiftmask_init();

	if(r_shift > 0)
		p = r<<r_shift & r_mask;
	else if(r_shift < 0)
        	p = r>>-r_shift & r_mask;
	else
		p |= r & r_mask;
	if(g_shift > 0)
		p |= g<<g_shift & g_mask;
	else if(g_shift < 0)
		p |= g>>-g_shift & g_mask;
	else
		p |= g & g_mask;
	if(b_shift > 0)
		p |= b<<b_shift & b_mask;
	else if(b_shift < 0)
		p |= b>>-b_shift & b_mask;
	else
		p |= b & b_mask;
	return p;
}

static void
st3_fixup(void)
{
	int x, y;
	uchar *src;
	PIXEL *dest;

	for(y = 0; y < vid.height; y++){
		src = &framebuf[y*vid.rowbytes];
		dest = (PIXEL*)src;

		/* vid.width % 8 not always 0
		for(x = vid.width-1; x >= 0; x -= 8) {
			dest[x  ] = st2d_8to24table[src[x  ]];
			dest[x-1] = st2d_8to24table[src[x-1]];
			dest[x-2] = st2d_8to24table[src[x-2]];
			dest[x-3] = st2d_8to24table[src[x-3]];
			dest[x-4] = st2d_8to24table[src[x-4]];
			dest[x-5] = st2d_8to24table[src[x-5]];
			dest[x-6] = st2d_8to24table[src[x-6]];
			dest[x-7] = st2d_8to24table[src[x-7]];
		}
		*/

		for(x = vid.width-1; x >= 0; x--)
			dest[x] = st2d_8to24table[src[x]];
	}
}

static void
resetfb(void)
{
	vid.width = Dx(screen->r);
	vid.height = Dy(screen->r);
	if(framebuf != nil){
		free(framebuf);
		framebuf = nil;
	}
	if(fbim != nil){
		freeimage(fbim);
		fbim = nil;
	}
	if((framebuf = malloc(sizeof *framebuf * vid.width * vid.height * screen->depth/8)) == nil)
		sysfatal("resetfb:malloc: %r");
	if((fbim = allocimage(display, Rect(0, 0, vid.width, vid.height), XRGB32, 0, 0)) == nil)
		sysfatal("resetfb: %r");
	vid.buffer = framebuf;
	vid.rowbytes = vid.width * screen->depth/8;
	center = addpt(screen->r.min, Pt(vid.width/2, vid.height/2));
	sw_mode->modified = true;	/* make ref_soft refresh its shit */
}

int
SWimp_Init(void *, void *)
{
	srand(getpid());

	if(initdraw(nil, nil, "quake2") < 0)
		sysfatal("VID_Init:initdraw: %r\n");
	resetfb();
	rwon = 1;
	vidref_val = VIDREF_SOFT;
	return 1;
}

/* copy backbuffer to front buffer */
void
SWimp_EndFrame(void)
{
	if(resized){		/* skip frame if window resizes */
		resized = 0;
		if(getwindow(display, Refnone) < 0)
			sysfatal("SWimp_EndFrame:getwindow: %r\n");
		resetfb();
		return;
	}
	st3_fixup();
	loadimage(fbim, fbim->r, framebuf, vid.height * vid.rowbytes);
	draw(screen, screen->r, fbim, nil, ZP);
	flushimage(display, 1);
}

rserr_t
SWimp_SetMode(int */*pwidth*/, int */*pheight*/, int /*mode*/, qboolean /*fullscreen*/)
{
	return rserr_ok;
}

/* nil palette == use existing; palette is expected to be in padded 4-byte xRGB format */
void
SWimp_SetPalette(uchar *palette)
{
	int i;

	if(!rwon)
		return;
	if(!palette)
        	palette = (uchar *)sw_state.currentpalette;
	for(i = 0; i < 256; i++)
		st2d_8to24table[i] = rgb24(palette[i*4], palette[i*4+1], palette[i*4+2]);
}

void
SWimp_Shutdown(void)
{
	if(!rwon)
		return;
	if(framebuf != nil)
		free(framebuf);
	if(fbim != nil)
		freeimage(fbim);
	rwon = 0;
}

void
SWimp_AppActivate(qboolean /*active*/)
{
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
VID_CheckChanges(void)
{
}

void
VID_Shutdown(void)
{
	R_Shutdown();
}

void
VID_Init(void)
{
	refimport_t ri;

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
	re.Init(nil, nil);
}
