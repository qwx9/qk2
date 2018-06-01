#include <u.h>
#include <libc.h>
#include <stdio.h>
#include "../dat.h"
#include "../fns.h"

void SP_crbot( char* name, int skill_level, char* skin, int team, char* model );
void cr_show_stats( edict_t *self );

#define MENU_MAIN     1
#define MENU_ADD_BOT  2

#define MI_MAIN_ADDBOT      1
#define MI_MAIN_STATS       2
#define MI_MAIN_TEAMS       3
#define MI_MAIN_MYTEAM      4
#define MI_MAIN_OBITUARY    5
#define MI_MAIN_BOTCHAT     6
#define MI_MAIN_TAUNT       7
#define MI_MAIN_MAPCYCLE    8
#define MI_MAIN_TECHS       9
#define MI_MAIN_HOOK        10
#define MI_MAIN_SAVENODES   11
#define MI_MAIN_MAX         11

#define MI_AB_PREVMENU   1
#define MI_AB_SKILL      2
#define MI_AB_MODEL      3
#define MI_AB_SKIN       4
#define MI_AB_TEAM       5 
#define MI_AB_ADDBOT     6
#define MI_AB_MAX        6

#define MAX_SKIN  1024
#define MAX_MODEL 128

typedef char stname[32];

stname skins[MAX_SKIN], models[MAX_MODEL];
int    nmodel=0, curmd=0, curskin=0;
int    skhead[MAX_MODEL], nskin[MAX_MODEL];

void cr_invert_string( char* pc )
{
	while(*pc!='"') {
		*pc |= 0x80;
		pc++;
	}
}

/* FIXME: bullshit */
void cr_menu_init(void)
{
	char mdir[256], sdir[512], *p;
	int fd, nmd, nsk, mdi, sdi, l, n, s_count, s_start = 0;
	Dir *md = nil, *sk = nil;

	memset(skins, 0, sizeof skins);
	memset(models, 0, sizeof models);
	seprint(mdir, mdir+sizeof mdir, "%s/baseq2/players", base_path->string);

	if((fd = open(mdir, OREAD)) < 0){
		fprint(2, "cr_menu_init:open: %r\n");
		goto fail;
	}
	nmd = dirreadall(fd, &md);
	close(fd);
	if(nmd <= 0){
		fprint(2, "cr_menu_init:dirreadall: %r\n");
		goto fail;
	}
	if(nmd > MAX_MODEL){
		fprint(2, "cr_menu_init: too many models\n");
		goto fail;
	}

	/* get models and skins */
	nmodel = 0;
	for(mdi=0; mdi < nmd; mdi++){
		/* [words]
			return thisdir->d_name[0] != '.';
			//return thisdir->d_type == DT_DIR; //always returns zero ?
		*/
		if(~md[mdi].mode & DMDIR)
			continue;

		strncpy(models[nmodel], md[mdi].name, sizeof *models);
		seprint(sdir, sdir+sizeof sdir, "%s/%s", mdir, models[nmodel]);
		if((fd = open(sdir, OREAD)) < 0){
			fprint(2, "cr_menu_init:open: %r\n");
			goto fail;
		}
		nsk = dirreadall(fd, &sk);
		close(fd);
		if(nsk <= 0){
			fprint(2, "cr_menu_init:dirreadall: %r\n");
			continue;
		}

		s_count = 0;
		for(sdi = 0; sdi < nsk; sdi++){
			if(strstr(sk[sdi].name, "_i.pcx") == nil)
				continue;
			p = skins[s_start+s_count];
			memset(p, 0, sizeof *p);
			l = strlen(sk[sdi].name) - 6;
			if(l > sizeof *p)
				l = sizeof *p;
			strncpy(p, sk[sdi].name, l);
			s_count++;
		}
		free(sk);
		skhead[nmodel] = s_start;
		nskin[nmodel] = s_count;
		s_start += s_count;
		nmodel++;
	}
	free(md);

	for(n=0; n < nmodel; n++)
		if(!cistrcmp(bot_model->string, models[n])){
			curmd = n;
			break;
		}
	for(n=0; n < nskin[curmd]; n++)
		if(!cistrcmp(bot_skin->string, skins[skhead[curmd]+n])){
			curskin = n;
			break;
		}
	gi.cvar_forceset("bot_model", models[curmd]);
	gi.cvar_forceset("bot_skin", skins[skhead[curmd]+curskin]);
	return;
fail:
	free(md);
	free(sk);
	gi.error("failed to init models and skins");
}

void cr_menu_draw( edict_t *self )
{
	char  menu[1400], *pch;
	gclient_t *cl;

	cl = self->client;
	if (!cl || self->bot_info) return;

	if (cl->menu_no<=0) cl->menu_no=1;
	if (cl->menu_item_no<=0) cl->menu_item_no=0;

	pch = menu;
	sprintf ( pch, "xv 32 yv 8 picn inventory " );
	pch += strlen(pch);

	switch (cl->menu_no) {
	case MENU_MAIN:
		sprintf( pch, "xv 0 yv 30 cstring2 \"[CRBot] main menu:\" "
			"xv 0 yv 32 cstring2 \"__________________\" " );
		break;
	case MENU_ADD_BOT:
		sprintf( pch, "xv 0 yv 30 cstring2 \"[CRBot] add new bot:\" "
			"xv 0 yv 32 cstring2 \"____________________\" " );
		break;
	default:
		cl->menu_no=0;
		return;
	};
	pch += strlen(pch);

	switch (cl->menu_no) {
	case MENU_MAIN:
		sprintf( pch, "xv 56 yv 50 string2 \"add new bot...\" " );
		if (cl->menu_item_no!=MI_MAIN_ADDBOT) cr_invert_string(pch+21);
		pch += strlen(pch);
		sprintf( pch, "xv 56 yv 60 string2 \"show bots statistics...\" " );
		if (cl->menu_item_no!=MI_MAIN_STATS) cr_invert_string(pch+21);
		pch += strlen(pch);
		sprintf( pch, "xv 56 yv 70 string2 \"show team scores...\" " );
		if (cl->menu_item_no!=MI_MAIN_TEAMS) cr_invert_string(pch+21);
		pch += strlen(pch);
		if (self->client->pers.team_no)
			sprintf( pch, "xv 56 yv 80 string2 \"player's team     = %d\" ", self->client->pers.team_no );
		else
			sprintf( pch, "xv 56 yv 80 string2 \"player's team     = none\" " );
		if (cl->menu_item_no!=MI_MAIN_MYTEAM) cr_invert_string(pch+21);
		pch += strlen(pch);
		sprintf( pch, "xv 56 yv 90 string2 \"obituary messages = %s\" ", obituary_msgs->value ? "show" : "skip" );
		if (cl->menu_item_no!=MI_MAIN_OBITUARY) cr_invert_string(pch+21);
		pch += strlen(pch);
		sprintf( pch, "xv 56 yv 100 string2 \"bot chat          = %s\" ", bot_chat->value ? "on" : "off" );
		if (cl->menu_item_no!=MI_MAIN_BOTCHAT) cr_invert_string(pch+22);
		pch += strlen(pch);
		sprintf( pch, "xv 56 yv 110 string2 \"bot taunts        = %s\" ", bot_taunt->value ? "on" : "off" );
		if (cl->menu_item_no!=MI_MAIN_TAUNT) cr_invert_string(pch+22);
		pch += strlen(pch);
		sprintf( pch, "xv 56 yv 120 string2 \"map cycle         = %s\" ", ((int)map_cycle->value)==2 ? "random" : ((int)map_cycle->value)==1 ? "on" : "off" );
		if (cl->menu_item_no!=MI_MAIN_MAPCYCLE) cr_invert_string(pch+22);
		pch += strlen(pch);
		sprintf( pch, "xv 56 yv 130 string2 \"techs in dm game  = %s\" ", !((int)no_tech->value) ? "enable" : "disable" );
		if (cl->menu_item_no!=MI_MAIN_TECHS) cr_invert_string(pch+22);
		pch += strlen(pch);
		sprintf( pch, "xv 56 yv 140 string2 \"g.hook in dm game = %s\" ", !((int)no_hook->value) ? "enable" : "disable" );
		if (cl->menu_item_no!=MI_MAIN_HOOK) cr_invert_string(pch+22);
		pch += strlen(pch);
		sprintf( pch, "xv 56 yv 150 string2 \"save nodemap file...\" " );
		if (cl->menu_item_no!=MI_MAIN_SAVENODES) cr_invert_string(pch+22);
		break;
	case MENU_ADD_BOT:
		sprintf( pch, "xv 56 yv 50 string2 \"...back to main menu...\" " );
		if (cl->menu_item_no!=MI_AB_PREVMENU) cr_invert_string(pch+21);
		pch += strlen(pch);
		if (bot_skill->value)
			sprintf( pch, "xv 56 yv 60 string2 \"skill  = %d\" ", (int)bot_skill->value );
		else
			sprintf( pch, "xv 56 yv 60 string2 \"skill  = adapting\" " );
		if (cl->menu_item_no!=MI_AB_SKILL) cr_invert_string(pch+21);
		pch += strlen(pch);
		sprintf( pch, "xv 56 yv 70 string2 \"model   = %s\" ", bot_model->string );
		if (cl->menu_item_no!=MI_AB_MODEL) cr_invert_string(pch+21);
		pch += strlen(pch);
		sprintf( pch, "xv 56 yv 80 string2 \"skin    = %s\" ", bot_skin->string );
		if (cl->menu_item_no!=MI_AB_SKIN) cr_invert_string(pch+21);
		pch += strlen(pch);
		if (bot_team->value)
			sprintf( pch, "xv 56 yv 90 string2 \"team    = %d\" ", (int)bot_team->value );
		else
			sprintf( pch, "xv 56 yv 90 string2 \"team    = none\" " );
		if (cl->menu_item_no!=MI_AB_TEAM) cr_invert_string(pch+21);
		pch += strlen(pch);
		sprintf( pch, "xv 56 yv 100 string2 \"-> add new bot <-\" " );
		if (cl->menu_item_no!=MI_AB_ADDBOT) cr_invert_string(pch+22);
		break;
	};

	gi.configstring ( CS_STATUSBAR, menu );
}


void cr_menu_select( edict_t *self, int ds )
{
	gclient_t *cl;
	int        no;
	char       buf[8];

	cl = self->client;
	if (!cl || self->bot_info) return;

	switch (cl->menu_no) {
	case MENU_MAIN:
		switch (cl->menu_item_no) {
		case MI_MAIN_ADDBOT:
			cl->menu_no = MENU_ADD_BOT;
			cl->menu_item_no = 1;
			break;
		case MI_MAIN_OBITUARY:
			gi.cvar_forceset( "obituary_msgs", obituary_msgs->value ? "0" : "1" );
			break;
		case MI_MAIN_MYTEAM:
			self->client->pers.team_no += ds;
			if (self->client->pers.team_no>10) self->client->pers.team_no=0;
			if (self->client->pers.team_no<0) self->client->pers.team_no=10;
			break;
		case MI_MAIN_STATS:
			cr_show_stats(self);
			break;
		case MI_MAIN_TEAMS:
			cr_show_team_stats(self);
			break;
		case MI_MAIN_BOTCHAT:
			gi.cvar_forceset( "bot_chat", bot_chat->value ? "0" : "1" );
			break;
		case MI_MAIN_TAUNT:
			gi.cvar_forceset( "bot_taunt", bot_taunt->value ? "0" : "1" );
			break;
		case MI_MAIN_MAPCYCLE:
			gi.cvar_forceset( "map_cycle", va( "%d", ((int)map_cycle->value+ds+3)%3 ) );
			break;
		case MI_MAIN_TECHS:
			gi.cvar_forceset( "no_tech", no_tech->value ? "0" : "1" );
			break;
		case MI_MAIN_HOOK:
			gi.cvar_forceset( "no_hook", no_hook->value ? "0" : "1" );
			break;
		case MI_MAIN_SAVENODES:
			cr_routes_save(self);
			break;
		}
		break;

	case MENU_ADD_BOT:
		switch (cl->menu_item_no) {
		case MI_AB_PREVMENU:
			cl->menu_no = MENU_MAIN;
			cl->menu_item_no = 1;
			break;
		case MI_AB_MODEL:
			curmd+=ds;
			if (curmd>=nmodel) curmd=0;
			if (curmd<0) curmd=nmodel-1;
			gi.cvar_forceset( "bot_model", models[curmd] );
			if (curskin>=nskin[curmd])
				curskin = nskin[curmd]-1;
			gi.cvar_forceset( "bot_skin", skins[skhead[curmd]+curskin] );
			break;
		case MI_AB_SKIN:
			if (curmd>=nmodel) curmd=nmodel-1;
			curskin+=ds;
			if (curskin>=nskin[curmd]) curskin=0;
			if (curskin<0) curskin = nskin[curmd]-1;
			gi.cvar_forceset( "bot_skin", skins[skhead[curmd]+curskin] );
			break;
		case MI_AB_TEAM:
			no = ((int)bot_team->value + ds + 10) % 10;
			sprintf(buf,"%d",no);
			gi.cvar_forceset( "bot_team", buf );
			break;
		case MI_AB_SKILL:
			no = ((int)bot_skill->value + ds + 10) % 10;
			sprintf(buf,"%d",no);
			gi.cvar_forceset( "bot_skill", buf );
			break;
		case MI_AB_ADDBOT:
			SP_crbot( NULL, bot_skill->value, bot_skin->string, bot_team->value, bot_model->string );
			break;
		}
		break;
	}

	cr_menu_draw(self);
}

void cr_menu_up( edict_t *self )
{
	gclient_t *cl;

	cl = self->client;
	if (!cl || self->bot_info) return;

	cl->menu_item_no--;
	if (cl->menu_item_no<=0)
		switch (cl->menu_no) {
		case MENU_MAIN:
			cl->menu_item_no=MI_MAIN_MAX;
			break;
		case MENU_ADD_BOT:
			cl->menu_item_no=MI_AB_MAX;
			break;
		default:
			cl->menu_item_no=1;
		}

	cr_menu_draw(self);
}

void cr_menu_down( edict_t *self )
{
	gclient_t *cl;

	cl = self->client;
	if (!cl || self->bot_info) return;

	cl->menu_item_no++;
	switch (cl->menu_no) {
	case MENU_MAIN:
		if (cl->menu_item_no>MI_MAIN_MAX) cl->menu_item_no=1;
		break;
	case MENU_ADD_BOT:
		if (cl->menu_item_no>MI_AB_MAX) cl->menu_item_no=1;
		break;
	default:
		cl->menu_item_no=1;
	}

	cr_menu_draw(self);
}

