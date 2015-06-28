#include <u.h>
#include <libc.h>
#include <stdio.h>
#include "dat.h"
#include "fns.h"

/* Dynamic variable tracking
 *
 * cvar_t variables are used to hold scalar or string variables that can be
 * changed or displayed at the console or prog code as well as accessed
 * directly in C code. The user can access cvars from the console in three ways:
 * r_draworder		prints the current value
 * r_draworder 0	sets the current value to 0
 * set r_draworder 0	as above, but creates the cvar if not present
 * Cvars are restricted from having the same names as commands to keep this
 * interface from being ambiguous. */

cvar_t *cvar_vars;

/* if set, each time a CVAR_USERINFO var is changed, the client will send it
 * to the server */
qboolean userinfo_modified;


static qboolean
Cvar_InfoValidate(char *s)
{
	if(strstr(s, "\\"))
		return false;
	if(strstr(s, "\""))
		return false;
	if(strstr(s, ";"))
		return false;
	return true;
}

static cvar_t *
Cvar_FindVar(char *name)
{
	cvar_t *p;

	for(p = cvar_vars; p != nil; p = p->next)
		if(!strcmp(name, p->name))
			return p;
	return nil;
}

float
Cvar_VariableValue(char *name)
{
	cvar_t *p;
	
	if((p = Cvar_FindVar(name)) == nil)
		return 0;
	return atof(p->string);
}

char *
Cvar_VariableString(char *name)
{
	cvar_t *p;

	if((p = Cvar_FindVar(name)) == nil)
		return "";
	return p->string;
}

char *
Cvar_CompleteVariable(char *partial)
{
	cvar_t *p;
	int len;

	len = strlen(partial);
	if(!len)
		return nil;

	/* check exact match */
	for(p = cvar_vars; p != nil; p = p->next)
		if(!strcmp(partial, p->name))
			return p->name;
	/* check partial match */
	for(p = cvar_vars; p != nil; p = p->next)
		if(!strncmp(partial, p->name, len))
			return p->name;
	return nil;
}

/* If the variable already exists, the value will not be set. The flags will be
 * or'ed in if the variable exists. */
cvar_t *
Cvar_Get(char *var_name, char *var_value, int flags)
{
	cvar_t *p;
	
	if(flags & (CVAR_USERINFO|CVAR_SERVERINFO)){
		if(!Cvar_InfoValidate(var_name)){
			Com_Printf("invalid info cvar name\n");
			return nil;
		}
	}

	if((p = Cvar_FindVar(var_name)) != nil){
		p->flags |= flags;
		return p;
	}

	if(!var_value)
		return nil;

	if(flags & (CVAR_USERINFO|CVAR_SERVERINFO)){
		if(!Cvar_InfoValidate(var_value)){
			Com_Printf("invalid info cvar value\n");
			return nil;
		}
	}

	p = Z_Malloc(sizeof(*p));
	p->name = CopyString(var_name);
	p->string = CopyString(var_value);
	p->modified = true;
	p->value = atof(p->string);
	p->next = cvar_vars;
	cvar_vars = p;
	p->flags = flags;
	return p;
}

cvar_t *
Cvar_Set2(char *var_name, char *value, qboolean force)
{
	cvar_t *var;

	var = Cvar_FindVar(var_name);
	if(!var)
		return Cvar_Get(var_name, value, 0);	// create it

	if(var->flags & (CVAR_USERINFO|CVAR_SERVERINFO)){
		if(!Cvar_InfoValidate(value)){
			Com_Printf("invalid info cvar value\n");
			return var;
		}
	}
	if(!force){
		if(var->flags & CVAR_NOSET){
			Com_Printf("%s is write protected.\n", var_name);
			return var;
		}
		if(var->flags & CVAR_LATCH){
			if(var->latched_string){
				if(!strcmp(value, var->latched_string))
					return var;
				Z_Free(var->latched_string);
			}else if(!strcmp(value, var->string))
					return var;

			if(Com_ServerState()){
				Com_Printf("%s will be changed for next game.\n", var_name);
				var->latched_string = CopyString(value);
			}else{
				var->string = CopyString(value);
				var->value = atof(var->string);
				if(!strcmp(var->name, "game")){
					FS_SetGamedir(var->string);
					FS_ExecAutoexec();
				}
			}
			return var;
		}
	}else{
		if(var->latched_string){
			Z_Free(var->latched_string);
			var->latched_string = nil;
		}
	}

	if(!strcmp(value, var->string))
		return var;	// not changed

	var->modified = true;

	if(var->flags & CVAR_USERINFO)
		userinfo_modified = true;	// transmit at next oportunity
	
	Z_Free(var->string);	// free the old value string
	var->string = CopyString(value);
	var->value = atof(var->string);
	return var;
}

/* set the variable even if NOSET or LATCH */
cvar_t *
Cvar_ForceSet(char *var_name, char *value)
{
	return Cvar_Set2(var_name, value, true);
}

cvar_t *
Cvar_Set(char *var_name, char *value)
{
	return Cvar_Set2(var_name, value, false);
}

cvar_t *
Cvar_FullSet(char *var_name, char *value, int flags)
{
	cvar_t *var;
	
	var = Cvar_FindVar(var_name);
	if(!var)
		return Cvar_Get(var_name, value, flags);	// create it

	var->modified = true;

	if(var->flags & CVAR_USERINFO)
		userinfo_modified = true;	// transmit at next oportunity

	Z_Free(var->string);	// free the old value string
	var->string = CopyString(value);
	var->value = atof(var->string);
	var->flags = flags;
	return var;
}

void
Cvar_SetValue(char *var_name, float value)
{
	char val[32];

	if(value == (int)value)
		Com_sprintf(val, sizeof val, "%d", (int)value);
	else
		Com_sprintf(val, sizeof val, "%f", value);
	Cvar_Set(var_name, val);
}

/* Any variables with latched values will now be updated */
void
Cvar_GetLatchedVars(void)
{
	cvar_t *p;

	for(p = cvar_vars; p != nil; p = p->next){
		if(!p->latched_string)
			continue;
		Z_Free(p->string);
		p->string = p->latched_string;
		p->latched_string = nil;
		p->value = atof(p->string);
		if(!strcmp(p->name, "game")){
			FS_SetGamedir(p->string);
			FS_ExecAutoexec();
		}
	}
}

/* called by Cmd_ExecuteString when Cmd_Argv(0) doesn't match a known command.
 * Returns true if the command was a variable reference that was handled (print
 * or change). */
qboolean
Cvar_Command(void)
{
	cvar_t *p;

	/* check variables */
	if((p = Cvar_FindVar(Cmd_Argv(0))) == nil)
		return false;

	/* perform a variable print or set */
	if(Cmd_Argc() == 1){
		Com_Printf("\"%s\" is \"%s\"\n", p->name, p->string);
		return true;
	}

	Cvar_Set(p->name, Cmd_Argv(1));
	return true;
}

/* Allows setting and defining of arbitrary cvars from console */
void
Cvar_Set_f(void)
{
	int c;
	int flags;

	c = Cmd_Argc();
	if(c != 3 && c != 4){
		Com_Printf("usage: set <variable> <value> [u / s]\n");
		return;
	}
	if(c == 4){
		if(!strcmp(Cmd_Argv(3), "u"))
			flags = CVAR_USERINFO;
		else if(!strcmp(Cmd_Argv(3), "s"))
			flags = CVAR_SERVERINFO;
		else{
			Com_Printf("flags can only be 'u' or 's'\n");
			return;
		}
		Cvar_FullSet(Cmd_Argv(1), Cmd_Argv(2), flags);
	}else
		Cvar_Set(Cmd_Argv(1), Cmd_Argv(2));
}

/* Appends lines containing "set variable value" for all variables with the
 * archive flag set to true. */
void
Cvar_WriteVariables(char *path)
{
	int fd;
	cvar_t *p;

	if((fd = open(path, OWRITE)) < 0){
		fprint(2, "Cvar_WriteVariables:open: %r\n");
		return;
	}
	seek(fd, 0, 2);
	for(p = cvar_vars; p != nil; p = p->next)
		if(p->flags & CVAR_ARCHIVE)
			if(fprint(fd, "set %s \"%s\"\n", p->name, p->string) < 0)
				sysfatal("Cvar_WriteVariables:fprint: %r");
	close(fd);
}

void
Cvar_List_f(void)
{
	cvar_t *p;
	int i;

	for(p = cvar_vars, i = 0; p != nil; p = p->next, i++){
		if(p->flags & CVAR_ARCHIVE)
			Com_Printf("*");
		else
			Com_Printf(" ");
		if(p->flags & CVAR_USERINFO)
			Com_Printf("U");
		else
			Com_Printf(" ");
		if(p->flags & CVAR_SERVERINFO)
			Com_Printf("S");
		else
			Com_Printf(" ");
		if(p->flags & CVAR_NOSET)
			Com_Printf("-");
		else if(p->flags & CVAR_LATCH)
			Com_Printf("L");
		else
			Com_Printf(" ");
		Com_Printf(" %s \"%s\"\n", p->name, p->string);
	}
	Com_Printf("%d cvars\n", i);
}

char *
Cvar_BitInfo(int bit)
{
	static char info[MAX_INFO_STRING];
	cvar_t *p;

	info[0] = 0;
	for(p = cvar_vars; p != nil; p = p->next)
		if(p->flags & bit)
			Info_SetValueForKey(info, p->name, p->string);
	return info;
}

/* returns an info string containing all the CVAR_USERINFO cvars */
char *
Cvar_Userinfo(void)
{
	return Cvar_BitInfo(CVAR_USERINFO);
}

/* returns an info string containing all the CVAR_SERVERINFO cvars */
char *
Cvar_Serverinfo(void)
{
	return Cvar_BitInfo(CVAR_SERVERINFO);
}

void
Cvar_Init(void)
{
	Cmd_AddCommand("set", Cvar_Set_f);
	Cmd_AddCommand("cvarlist", Cvar_List_f);
}
