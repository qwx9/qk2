#include <u.h>
#include <libc.h>
#include <stdio.h>
#include <draw.h>
#include <thread.h>
#include <mouse.h>
#include <keyboard.h>
#include "../dat.h"
#include "../fns.h"

cvar_t	*in_mouse;
cvar_t	*in_joystick;
cvar_t	*m_filter;
cvar_t	*m_windowed;
cvar_t	*sensitivity;
cvar_t	*lookstrafe;
cvar_t	*m_side;
cvar_t	*m_yaw;
cvar_t	*m_pitch;
cvar_t	*m_forward;
cvar_t	*freelook;

qboolean mouseon;
qboolean mlooking;
int mx, my, mbtn, dx, dy, oldmx, oldmy, oldmbtn;
int oldmwin;

typedef struct Kev Kev;
struct Kev{
	int key;
	int down;
};
enum{
	Nbuf	= 64
};
Channel *kchan;
int ktid = -1, mtid = -1;

/* rw_9.c */
extern int resized;
extern Point center;
extern refimport_t ri;

void IN_Grabm(int on)
{
	static char nocurs[2*4+2*2*16];
	static int fd = -1;

	if(mouseon == on)
		return;
	if(mouseon = on && m_windowed->value){
		if((fd = open("/dev/cursor", ORDWR|OCEXEC)) < 0){
			sysfatal("IN_Grabm:open: %r\n");
			return;
		}
		write(fd, nocurs, sizeof nocurs);
	}else if(fd >= 0){
		close(fd);
		fd = -1;
	}
}

void IN_Commands (void)
{
	int i;

	if(!mouseon)
		return;
	for(i = 0; i < 3; i++){
		if(mbtn & 1<<i && ~oldmbtn & 1<<i)
			Key_Event(K_MOUSE1+i, true, Sys_Milliseconds());
		if(~mbtn & 1<<i && oldmbtn & 1<<i)
			Key_Event(K_MOUSE1+i, false, Sys_Milliseconds());
	}
	oldmbtn = mbtn;
}

void KBD_Update (void)
{
	Kev ev;
	int r;

	if(oldmwin != m_windowed->value){
		oldmwin = m_windowed->value;
		IN_Grabm(m_windowed->value);
	}
	while((r = nbrecv(kchan, &ev)) > 0)
		Key_Event(ev.key, ev.down, Sys_Milliseconds());
	if(r < 0)
		sysfatal("KBD_Update:nbrecv: %r\n");
}

void IN_Move (usercmd_t *cmd)
{
	if(!mouseon)
		return;

	if(m_filter->value){
		mx = (mx + oldmx) * 0.5;
		my = (my + oldmy) * 0.5;
	}else{
		mx = dx;
		my = dy;
	}
	oldmx = dx;
	oldmy = dy;
	if(!mx && !my)
		return;
	mx *= sensitivity->value;
	my *= sensitivity->value;
	dx = dy = 0;

	/* add mouse x/y movement to cmd */
	if(in_strafe.state & 1 || lookstrafe->value && mlooking)
		cmd->sidemove += m_side->value * mx;
	else
		cl.viewangles[YAW] -= m_yaw->value * mx;
	if((mlooking || freelook->value) && ~in_strafe.state & 1)
		cl.viewangles[PITCH] += m_pitch->value * my;
	else
		cmd->forwardmove -= m_forward->value * my;
}

/* called on focus/unfocus in win32 */
void IN_Activate (qboolean)
{
}

/* called every frame even if not generating commands */
void IN_Frame (void)
{
}

void IN_ForceCenterView (void)
{
	cl.viewangles[PITCH] = 0;
}

void IN_MLookDown (void)
{
	mlooking = true;
}

void IN_MLookUp (void)
{
	mlooking = false;
	IN_CenterView();
}

int runetokey (Rune r)
{
	int k = 0;

	switch(r){
	case Kpgup:	k = K_PGUP; break;
	case Kpgdown:	k = K_PGDN; break;
	case Khome:	k = K_HOME; break;
	case Kend:	k = K_END; break;
	case Kleft:	k = K_LEFTARROW; break;
	case Kright:	k = K_RIGHTARROW; break;
	case Kdown:	k = K_DOWNARROW; break;
	case Kup:	k = K_UPARROW; break;
	case Kesc:	k = K_ESCAPE; break;
	case '\n':	k = K_ENTER; break;
	case '\t':	k = K_TAB; break;
	case KF|1:	k = K_F1; break;
	case KF|2:	k = K_F2; break;
	case KF|3:	k = K_F3; break;
	case KF|4:	k = K_F4; break;
	case KF|5:	k = K_F5; break;
	case KF|6:	k = K_F6; break;
	case KF|7:	k = K_F7; break;
	case KF|8:	k = K_F8; break;
	case KF|9:	k = K_F9; break;
	case KF|10:	k = K_F10; break;
	case KF|11:	k = K_F11; break;
	case KF|12:	k = K_F12; break;
	case Kbs:	k = K_BACKSPACE; break;
	case Kdel:	k = K_DEL; break;
	case Kbreak:	k = K_PAUSE; break;
	case Kshift:	k = K_SHIFT; break;
	case Kctl:	k = K_CTRL; break;
	case Kalt:
	case Kaltgr:	k = K_ALT; break;
	case Kins:	k = K_INS; break;
	default:
		if(r < 0x80)
			k = r;
	};
	return k;
}

void kproc (void *)
{
	int n, k, fd;
	char buf[128], kdown[128] = {0}, *s;
	Rune r;
	Kev ev;

	if((fd = open("/dev/kbd", OREAD)) < 0)
		sysfatal("open /dev/kbd: %r");

	while((n = read(fd, buf, sizeof buf)) > 0){
		buf[n-1] = 0;
		switch(*buf){
		case 'c':
		default:
			continue;
		case 'k':
			s = buf+1;
			while(*s){
				s += chartorune(&r, s);
				if(utfrune(kdown+1, r) == nil){
					if(k = runetokey(r)){
						ev.key = k;
						ev.down = true;
						if(nbsend(kchan, &ev) < 0)
							sysfatal("kproc:nbsend: %r\n");
					}
				}
			}
			break;
		case 'K':
			s = kdown+1;
			while(*s){
				s += chartorune(&r, s);
				if(utfrune(buf+1, r) == nil){
					if(k = runetokey(r)){
						ev.key = k;
						ev.down = false;
						if(nbsend(kchan, &ev) < 0)
							sysfatal("mproc:nbsend: %r\n");
					}
				}
			}
			break;
		}
		strcpy(kdown, buf);
	}
	close(fd);
	ktid = -1;
}

void mproc (void *)
{
	int b, n, nerr = 0, fd;
	char buf[1+5*12];
	int x, y;

	if((fd = open("/dev/mouse", ORDWR)) < 0)
		sysfatal("open /dev/mouse: %r");

	for(;;){
		if((n = read(fd, buf, sizeof buf)) != 1+4*12){
			fprint(2, "mproc:read: bad count %d not 49: %r\n", n);
			if(n < 0 || ++nerr > 10)
				break;
			continue;
		}
		nerr = 0;
		switch(*buf){
		case 'r':
			resized = 1;
			/* fall through */
		case 'm':
			if(!mouseon)
				break;

			x = atoi(buf+1+0*12) - center.x;
			y = atoi(buf+1+1*12) - center.y;
			b = atoi(buf+1+2*12);

			mbtn = b&1 | (b&2)<<1 | (b&4)>>1;
			dx += x;
			dy += y;
			if(x != 0 || y != 0)
				fprint(fd, "m%d %d", center.x, center.y);
			break;
		}
	}
	close(fd);
	mtid = -1;
}

void IN_Shutdown (void)
{
	IN_Grabm(0);
	if(ktid != -1){
		threadkill(ktid);
		ktid = -1;
	}
	if(mtid != -1){
		threadkill(mtid);
		mtid = -1;
	}
	if(kchan != nil){
		chanfree(kchan);
		kchan = nil;
	}
	mouseon = false;
}

void IN_Init (void)
{
	in_mouse = ri.Cvar_Get("in_mouse", "1", CVAR_ARCHIVE);
	in_joystick = ri.Cvar_Get("in_joystick", "0", CVAR_ARCHIVE);
	m_windowed = ri.Cvar_Get("m_windowed", "0", CVAR_ARCHIVE);
	m_filter = ri.Cvar_Get("m_filter", "0", 0);
	freelook = ri.Cvar_Get("freelook", "0", 0);
	lookstrafe = ri.Cvar_Get("lookstrafe", "0", 0);
	sensitivity = ri.Cvar_Get("sensitivity", "3", 0);
	m_pitch = ri.Cvar_Get("m_pitch", "0.022", 0);
	m_yaw = ri.Cvar_Get("m_yaw", "0.022", 0);
	m_forward = ri.Cvar_Get("m_forward", "1", 0);
	m_side = ri.Cvar_Get("m_side", "0.8", 0);

	ri.Cmd_AddCommand("+mlook", IN_MLookDown);
	ri.Cmd_AddCommand("-mlook", IN_MLookUp);
	ri.Cmd_AddCommand("force_centerview", IN_ForceCenterView);

	kchan = chancreate(sizeof(Kev), Nbuf);
	if((ktid = proccreate(kproc, nil, 8192)) < 0)
		sysfatal("proccreate kproc: %r");
	if((mtid = proccreate(mproc, nil, 8192)) < 0)
		sysfatal("proccreate mproc: %r");
	mx = my = 0;
}
