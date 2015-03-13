#include <u.h>
#include <libc.h>
#include <stdio.h>
#include "../dat.h"
#include "../fns.h"

refexport_t	GetRefAPI(refimport_t);

cvar_t *vid_gamma;
cvar_t *vid_ref;		// Name of Refresh DLL loaded
cvar_t *vid_xpos;		// X coordinate of window position
cvar_t *vid_ypos;		// Y coordinate of window position
cvar_t *vid_fullscreen;
refexport_t re;			/* exported functions from refresh DLL */


void VID_Printf (int print_level, char *fmt, ...)
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

void VID_Error (int err_level, char *fmt, ...)
{
	va_list argptr;
	char msg[4096];
	
	va_start(argptr, fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);
	Com_Error(err_level, "%s", msg);
}

void VID_CheckChanges (void)
{
}

void VID_Shutdown (void)
{
	re.Shutdown();
}

void VID_Init (void)
{
	refimport_t ri;

	vid_ref = Cvar_Get("vid_ref", "libdraw", CVAR_ARCHIVE);
	vid_xpos = Cvar_Get("vid_xpos", "3", CVAR_ARCHIVE);
	vid_ypos = Cvar_Get("vid_ypos", "22", CVAR_ARCHIVE);
	vid_fullscreen = Cvar_Get("vid_fullscreen", "0", CVAR_ARCHIVE);
	vid_gamma = Cvar_Get("vid_gamma", "1", CVAR_ARCHIVE);

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
