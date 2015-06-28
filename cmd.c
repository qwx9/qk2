#include <u.h>
#include <libc.h>
#include <stdio.h>
#include "dat.h"
#include "fns.h"

/* Command text buffering and command execution
 *
 * Any number of commands can be added in a frame, from several different
 * sources. Most commands come from either keybindings or console line input,
 * but remote servers can also send across commands and entire text files can
 * be execed. The '+' command line options are also added to the command buffer.
 * The game starts with a Cbuf_AddText("exec quake.rc\n"); Cbuf_Execute();.
 * Command execution takes a null terminated string, breaks it into tokens,
 * then searches for a command or variable that matches the first token. */

typedef struct cmdalias_t cmdalias_t;
typedef struct cmd_function_t cmd_function_t;

enum{
	MAX_ALIAS_NAME = 32,
	ALIAS_LOOP_COUNT = 16
};
struct cmdalias_t{
	cmdalias_t *next;
	char name[MAX_ALIAS_NAME];
	char *value;
};
cmdalias_t *cmd_alias;
int alias_count;	// for detecting runaway loops

qboolean cmd_wait;

sizebuf_t cmd_text;
uchar cmd_text_buf[8192];
uchar defer_text_buf[8192];

static int cmd_argc;
static char *cmd_argv[MAX_STRING_TOKENS];
static char *cmd_null_string = "";
static char cmd_args[MAX_STRING_CHARS];

struct cmd_function_t{
	cmd_function_t *next;
	char *name;
	xcommand_t function;
};
static cmd_function_t *cmd_functions;	// possible commands to execute


/* Causes execution of the remainder of the command buffer to be delayed until
 * next frame. This allows commands like:
 * bind g "impulse 5 ; +attack ; wait ; -attack ; impulse 2" */
void
Cmd_Wait_f(void)
{
	cmd_wait = true;
}

/* allocates an initial text buffer that will grow as needed */
void
Cbuf_Init(void)
{
	SZ_Init(&cmd_text, cmd_text_buf, sizeof cmd_text_buf);
}

/* adds command text at the end of the buffer */
void
Cbuf_AddText(char *text)
{
	int l;

	l = strlen(text);

	if(cmd_text.cursize + l >= cmd_text.maxsize){
		Com_Printf("Cbuf_AddText: overflow\n");
		return;
	}
	SZ_Write(&cmd_text, text, l);
}

// FIXME: actually change the command buffer to do less copying
/* Adds command text immediately after the current command and '\n' to the text */
void
Cbuf_InsertText(char *text)
{
	char *temp = nil;
	int templen;

	/* copy off any commands still remaining in the exec buffer */
	templen = cmd_text.cursize;
	if(templen){
		temp = Z_Malloc(templen);
		memcpy(temp, cmd_text.data, templen);
		SZ_Clear(&cmd_text);
	}

	Cbuf_AddText(text);
	if(templen){
		SZ_Write(&cmd_text, temp, templen);
		Z_Free(temp);
	}
}

void
Cbuf_CopyToDefer(void)
{
	memcpy(defer_text_buf, cmd_text_buf, cmd_text.cursize);
	defer_text_buf[cmd_text.cursize] = 0;
	cmd_text.cursize = 0;
}

void
Cbuf_InsertFromDefer(void)
{
	Cbuf_InsertText((char *)defer_text_buf);
	defer_text_buf[0] = 0;
}

void
Cbuf_ExecuteText(int exec_when, char *text)
{
	switch(exec_when){
	case EXEC_NOW:
		Cmd_ExecuteString(text);
		break;
	case EXEC_INSERT:
		Cbuf_InsertText(text);
		break;
	case EXEC_APPEND:
		Cbuf_AddText(text);
		break;
	default:
		Com_Error(ERR_FATAL, "Cbuf_ExecuteText: bad exec_when");
	}
}

/* normally called once per frame, but may be explicitly invoked.
 * do not call inside a command function! */
void
Cbuf_Execute(void)
{
	int i, quotes;
	char *text, line[1024];

	alias_count = 0;	// don't allow infinite alias loops

	while(cmd_text.cursize){
		text = (char *)cmd_text.data;

		/* find a \n or ; line break */
		quotes = 0;
		for(i=0; i<cmd_text.cursize; i++){
			if(text[i] == '"')
				quotes++;
			if(~quotes & 1 && text[i] == ';')
				break;	// don't break if inside a quoted string
			if(text[i] == '\n')
				break;
		}	
		memcpy(line, text, i);
		line[i] = 0;

		/* delete the text from the command buffer and move remaining
		 * commands down. this is necessary because commands (exec,
		 * alias) can insert data at the beginning of the text buffer */
		if(i == cmd_text.cursize)
			cmd_text.cursize = 0;
		else{
			i++;
			cmd_text.cursize -= i;
			memmove(text, text+i, cmd_text.cursize);
		}

		Cmd_ExecuteString(line);

		/* skip out while text still remains in buffer, leaving it for
		 * next frame */
		if(cmd_wait){
			cmd_wait = false;
			break;
		}
	}
}

/* Adds command line parameters as script statements. Commands lead with a '+',
 * and continue until another '+'. Set commands are added early, so they are
 * guaranteed to be set before the client and server initialize for the first
 * time. Other commands are added late, after all initialization is complete. */
void
Cbuf_AddEarlyCommands(qboolean clear)
{
	int i;
	char *s;

	for(i=0; i<COM_Argc(); i++){
		s = COM_Argv(i);
		if(strcmp(s, "+set"))
			continue;
		Cbuf_AddText(va("set %s %s\n", COM_Argv(i+1), COM_Argv(i+2)));
		if(clear){
			COM_ClearArgv(i);
			COM_ClearArgv(i+1);
			COM_ClearArgv(i+2);
		}
		i += 2;
	}
}

/* Adds command line parameters as script statements. Commands lead with a '+'
 * and continue until another '+' or '-'. Returns true if any late commands
 * were added, which will keep the demoloop from immediately starting. */
qboolean
Cbuf_AddLateCommands(void)
{
	int i, j, s, argc;
	char *text, *build, c;
	qboolean ret;

	/* build the combined string to parse from */
	s = 0;
	argc = COM_Argc();
	for(i=1; i<argc; i++)
		s += strlen(COM_Argv(i)) + 1;
	if(!s)
		return false;
		
	text = Z_Malloc(s+1);
	text[0] = 0;
	for(i=1; i<argc; i++){
		strcat(text, COM_Argv(i));
		if(i != argc-1)
			strcat(text, " ");
	}

	/* pull out the commands */
	build = Z_Malloc(s+1);
	build[0] = 0;
	for(i=0; i<s-1; i++)
		if(text[i] == '+'){
			i++;
			for(j=i; text[j] != '+' && text[j] != '-' && text[j] != 0 ; j++)
				;
			c = text[j];
			text[j] = 0;
			strcat(build, text+i);
			strcat(build, "\n");
			text[j] = c;
			i = j-1;
		}

	ret = build[0] != 0;
	if(ret)
		Cbuf_AddText(build);
	Z_Free(text);
	Z_Free(build);
	return ret;
}

void
Cmd_Exec_f(void)
{
	char *f, *f2;
	int len;

	if(Cmd_Argc() != 2){
		Com_Printf("exec <filename> : execute a script file\n");
		return;
	}

	len = FS_LoadFile(Cmd_Argv(1), (void **)&f);
	if(!f){
		Com_Printf("couldn't exec %s\n", Cmd_Argv(1));
		return;
	}
	Com_Printf("execing %s\n", Cmd_Argv(1));

	/* the file doesn't have a trailing 0, so we need to copy it off */
	f2 = Z_Malloc(len+1);
	memcpy(f2, f, len);
	f2[len] = 0;
	Cbuf_InsertText(f2);
	Z_Free(f2);
	FS_FreeFile(f);
}

/* just prints the rest of the line to the console */
void
Cmd_Echo_f(void)
{
	int i;
	
	for(i=1; i<Cmd_Argc(); i++)
		Com_Printf("%s ", Cmd_Argv(i));
	Com_Printf("\n");
}

/* Creates a new command that executes a command string (possibly ; seperated) */
void
Cmd_Alias_f(void)
{
	int i, c;
	char cmd[1024], *s;
	cmdalias_t *p;

	if(Cmd_Argc() == 1){
		Com_Printf("Current alias commands:\n");
		for(p = cmd_alias; p != nil; p = p->next)
			Com_Printf("%s : %s\n", p->name, p->value);
		return;
	}

	s = Cmd_Argv(1);
	if(strlen(s) >= MAX_ALIAS_NAME){
		Com_Printf("Alias name is too long\n");
		return;
	}

	/* if the alias already exists, reuse it */
	for(p = cmd_alias; p != nil; p = p->next)
		if(!strcmp(s, p->name)){
			Z_Free(p->value);
			break;
		}

	if(p == nil){
		p = Z_Malloc(sizeof *p);
		p->next = cmd_alias;
		cmd_alias = p;
	}
	strcpy(p->name, s);	

	/* copy the rest of the command line */
	cmd[0] = 0;	// start out with a null string
	c = Cmd_Argc();
	for(i=2; i<c; i++){
		strcat(cmd, Cmd_Argv(i));
		if(i != c-1)
			strcat(cmd, " ");
	}
	strcat(cmd, "\n");
	p->value = CopyString(cmd);
}

int
Cmd_Argc(void)
{
	return cmd_argc;
}

char *
Cmd_Argv(int arg)
{
	if((unsigned)arg >= cmd_argc)
		return cmd_null_string;
	return cmd_argv[arg];	
}

/* returns a single string containing argv(1) to argv(argc()-1) */
char *
Cmd_Args(void)
{
	return cmd_args;
}

char *
Cmd_MacroExpandString(char *text)
{
	int i, j, count, len;
	qboolean inquote;
	char *scan, *token, *start;
	char temporary[MAX_STRING_CHARS];
	static char expanded[MAX_STRING_CHARS];

	inquote = false;
	scan = text;
	len = strlen (scan);
	if(len >= MAX_STRING_CHARS){
		Com_Printf("Line exceeded %d chars, discarded.\n", MAX_STRING_CHARS);
		return nil;
	}

	count = 0;
	for(i=0; i<len ; i++){
		if(scan[i] == '"')
			inquote ^= 1;
		if(inquote)
			continue;	// don't expand inside quotes
		if(scan[i] != '$')
			continue;

		/* scan out the complete macro */
		start = scan+i+1;
		token = COM_Parse(&start);
		if(!start)
			continue;

		token = Cvar_VariableString(token);
		j = strlen(token);
		len += j;
		if(len >= MAX_STRING_CHARS){
			Com_Printf("Expanded line exceeded %d chars, discarded.\n", MAX_STRING_CHARS);
			return nil;
		}

		strncpy(temporary, scan, i);
		strcpy(temporary+i, token);
		strcpy(temporary+i+j, start);
		strcpy(expanded, temporary);
		scan = expanded;
		i--;

		if(++count == 100){
			Com_Printf("Macro expansion loop, discarded.\n");
			return nil;
		}
	}

	if(inquote){
		Com_Printf("Line has unmatched quote, discarded.\n");
		return nil;
	}
	return scan;
}

/* Parses the given string into command line tokens. $Cvars will be expanded
 * unless they are in a quoted token */
void
Cmd_TokenizeString(char *text, qboolean macroExpand)
{
	int i, l;
	char *com_token;

	/* clear the args from the last string */
	for(i=0; i<cmd_argc; i++)
		Z_Free(cmd_argv[i]);
	cmd_argc = 0;
	cmd_args[0] = 0;

	/* macro expand the text */
	if(macroExpand)
		text = Cmd_MacroExpandString(text);
	if(text == nil)
		return;

	for(;;){
		while(*text && *text <= ' ' && *text != '\n')
			text++;
		
		/* a newline separates commands in the buffer */
		if(*text == '\n'){
			text++;
			break;
		}

		if(!*text)
			return;

		// set cmd_args to everything after the first arg
		if(cmd_argc == 1){
			strcpy(cmd_args, text);
			/* strip off any trailing whitespace */
			for(l = strlen(cmd_args) - 1; l >= 0 ; l--)
				if(cmd_args[l] <= ' ')
					cmd_args[l] = 0;
				else
					break;
		}

		com_token = COM_Parse(&text);
		if(text == nil)
			return;

		if(cmd_argc < MAX_STRING_TOKENS){
			cmd_argv[cmd_argc] = Z_Malloc(strlen(com_token)+1);
			strcpy(cmd_argv[cmd_argc], com_token);
			cmd_argc++;
		}
	}
}

/* called by the init functions of other parts of the program to register
 * commands and functions to call for them. rhe cmd_name is referenced later,
 * so it should not be in temp memory  if function is nil, the command will be
 * forwarded to the server as a clc_stringcmd instead of executed locally */
void
Cmd_AddCommand(char *name, xcommand_t function)
{
	cmd_function_t *p;
	
	/* fail if the command is a variable name */
	if(Cvar_VariableString(name)[0]){
		Com_Printf("Cmd_AddCommand: %s already defined as a var\n", name);
		return;
	}

	/* fail if the command already exists */
	for(p = cmd_functions; p != nil; p = p->next)
		if(!strcmp(name, p->name)){
			Com_Printf("Cmd_AddCommand: %s already defined\n", name);
			return;
		}

	p = Z_Malloc(sizeof *p);
	p->name = name;
	p->function = function;
	p->next = cmd_functions;
	cmd_functions = p;
}

void
Cmd_RemoveCommand(char *name)
{
	cmd_function_t *p, **back;

	back = &cmd_functions;
	for(;;){
		p = *back;
		if(p == nil){
			Com_Printf("Cmd_RemoveCommand: %s not added\n", name);
			return;
		}
		if(!strcmp(name, p->name)){
			*back = p->next;
			Z_Free(p);
			return;
		}
		back = &p->next;
	}
}

/* used by the cvar code to check for cvar / command name overlap */
qboolean
Cmd_Exists(char *name)
{
	cmd_function_t *p;

	for(p = cmd_functions; p != nil; p = p->next)
		if(!strcmp(name, p->name))
			return true;
	return false;
}

/* attempts to match a partial command for automatic command line completion */
char *
Cmd_CompleteCommand(char *partial)
{
	cmd_function_t *p;
	int len;
	cmdalias_t *a;
	
	len = strlen(partial);
	if(!len)
		return nil;

	/* check for exact match */
	for(p = cmd_functions; p != nil; p = p->next)
		if(!strcmp(partial, p->name))
			return p->name;
	for(a = cmd_alias; a != nil; a = a->next)
		if(!strcmp(partial, a->name))
			return a->name;

	/* check for partial match */
	for(p = cmd_functions; p != nil; p = p->next)
		if(!strncmp(partial, p->name, len))
			return p->name;
	for(a = cmd_alias; a != nil; a = a->next)
		if(!strncmp(partial, a->name, len))
			return a->name;
	return nil;
}

// FIXME: lookupnoadd the token to speed search?
/* Parses a single line of text into arguments and tries to execute it as if it
 * was typed at the console */
void
Cmd_ExecuteString(char *text)
{	
	cmd_function_t *p;
	cmdalias_t *a;

	Cmd_TokenizeString(text, true);
	if(!Cmd_Argc())
		return;	// no tokens

	/* check functions */
	for(p = cmd_functions; p != nil; p = p->next)
		if(!cistrcmp(cmd_argv[0], p->name)){
			if(!p->function)	// forward to server command
				Cmd_ExecuteString(va("cmd %s", text));
			else
				p->function();
			return;
		}
	/* check alias */
	for(a = cmd_alias; a != nil; a = a->next)
		if(!cistrcmp(cmd_argv[0], a->name)){
			if(++alias_count == ALIAS_LOOP_COUNT){
				Com_Printf("ALIAS_LOOP_COUNT\n");
				return;
			}
			Cbuf_InsertText(a->value);
			return;
		}
	/* check cvars */
	if(Cvar_Command())
		return;

	/* send it as a server command if we are connected */
	Cmd_ForwardToServer();
}

void
Cmd_List_f(void)
{
	cmd_function_t *p;
	int i;

	for(p = cmd_functions, i = 0; p != nil; p = p->next, i++)
		Com_Printf("%s\n", p->name);
	Com_Printf("%d commands\n", i);
}

void
Cmd_Init(void)
{
	Cmd_AddCommand("cmdlist", Cmd_List_f);
	Cmd_AddCommand("exec", Cmd_Exec_f);
	Cmd_AddCommand("echo", Cmd_Echo_f);
	Cmd_AddCommand("alias", Cmd_Alias_f);
	Cmd_AddCommand("wait", Cmd_Wait_f);
}
