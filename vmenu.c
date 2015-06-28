#include <u.h>
#include <libc.h>
#include <stdio.h>
#include "dat.h"
#include "fns.h"

extern void M_PopMenu(void);

static menuframework_t vmenu;
static menuslider_t ssizeslide, gammaslide;
static menulist_t fullscrbox;
static menuaction_t applyaction, defaultsaction;


void
vmssize(void *s)
{
	Cvar_SetValue("viewsize", ((menuslider_t *)s)->curvalue * 10);
}

void
vmgamma(void *s)
{
	// invert sense so greater = brighter, and scale to a range of 0.5 to 1.3
	Cvar_SetValue("vid_gamma", 0.8 - (((menuslider_t *)s)->curvalue/10.0 - 0.5) + 0.5);
}

void
vmreset(void *)
{
	VID_MenuInit();
}

void
vmapply(void *)
{
	Cvar_SetValue("vid_gamma", 0.8 - (gammaslide.curvalue/10.0 - 0.5) + 0.5);
	Cvar_SetValue("vid_fullscreen", fullscrbox.curvalue);
	M_ForceMenuOff();
}

void
VID_MenuInit(void)
{
	static char *yesno[] = {"no", "yes", nil};

	if(!scr_viewsize)
		scr_viewsize = Cvar_Get("viewsize", "100", CVAR_ARCHIVE);
	ssizeslide.curvalue = scr_viewsize->value/10;

	vmenu.x = vid.width * 0.50;
	vmenu.nitems = 0;

	ssizeslide.generic.type = MTYPE_SLIDER;
	ssizeslide.generic.x = 0;
	ssizeslide.generic.y = 20;
	ssizeslide.generic.name = "screen size";
	ssizeslide.minvalue = 3;
	ssizeslide.maxvalue = 12;
	ssizeslide.generic.callback = vmssize;

	gammaslide.generic.type = MTYPE_SLIDER;
	gammaslide.generic.x = 0;
	gammaslide.generic.y = 30;
	gammaslide.generic.name = "gamma";
	gammaslide.generic.callback = vmgamma;
	gammaslide.minvalue = 5;
	gammaslide.maxvalue = 13;
	gammaslide.curvalue = (1.3 - vid_gamma->value + 0.5) * 10;

	fullscrbox.generic.type = MTYPE_SPINCONTROL;
	fullscrbox.generic.x = 0;
	fullscrbox.generic.y = 40;
	fullscrbox.generic.name = "fullscreen";
	fullscrbox.itemnames = yesno;
	fullscrbox.curvalue = vid_fullscreen->value;

	defaultsaction.generic.type = MTYPE_ACTION;
	defaultsaction.generic.name = "reset to default";
	defaultsaction.generic.x = 0;
	defaultsaction.generic.y = 90;
	defaultsaction.generic.callback = vmreset;

	applyaction.generic.type = MTYPE_ACTION;
	applyaction.generic.name = "apply";
	applyaction.generic.x = 0;
	applyaction.generic.y = 100;
	applyaction.generic.callback = vmapply;

	Menu_AddItem(&vmenu, (void *)&ssizeslide);
	Menu_AddItem(&vmenu, (void *)&gammaslide);
	Menu_AddItem(&vmenu, (void *)&fullscrbox);
	Menu_AddItem(&vmenu, (void *)&defaultsaction);
	Menu_AddItem(&vmenu, (void *)&applyaction);

	Menu_Center(&vmenu);
	vmenu.x -= 8;
}

void
VID_MenuDraw(void)
{
	int w, h;

	Draw_GetPicSize(&w, &h, "m_banner_video");
	Draw_Pic(vid.width/2 - w/2, vid.height/2 - 110, "m_banner_video");
	Menu_AdjustCursor(&vmenu, 1);	// starting position
	Menu_Draw(&vmenu);
}

char *VID_MenuKey (int key)
{
	static char *sound = "misc/menu1.wav";

	switch(key){
	case K_ESCAPE:
		M_PopMenu();
		return NULL;
	case K_UPARROW:
		vmenu.cursor--;
		Menu_AdjustCursor(&vmenu, -1);
		break;
	case K_DOWNARROW:
		vmenu.cursor++;
		Menu_AdjustCursor(&vmenu, 1);
		break;
	case K_LEFTARROW:
		Menu_SlideItem(&vmenu, -1);
		break;
	case K_RIGHTARROW:
		Menu_SlideItem(&vmenu, 1);
		break;
	case K_ENTER:
		Menu_SelectItem(&vmenu);
		break;
	}
	return sound;
}
