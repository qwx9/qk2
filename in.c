#include <u.h>
#include <libc.h>
#include <stdio.h>
#include <draw.h>
#include <thread.h>
#include <mouse.h>
#include <keyboard.h>
#include "dat.h"
#include "fns.h"

extern int resized;
extern Point center;
extern Channel *fuckchan, *tchan;
extern Rectangle grabr;

cvar_t *in_joystick;
cvar_t *sensitivity;
cvar_t *lookstrafe;
cvar_t *lookspring;
cvar_t *freelook;
cvar_t *m_pitch;

static cvar_t *m_filter;
static cvar_t *m_windowed;
static cvar_t *m_yaw;
static cvar_t *m_side;
static cvar_t *m_forward;

static int mouseon, fixms;
static int mlooking;
static int dx, dy;
static int oldmwin;

typedef struct Kev Kev;
struct Kev{
	int key;
	int down;
};
enum{
	Nbuf	= 64
};
static Channel *kchan, *mchan;
static int iop = -1, pfd[2];
static QLock killock;


char *
Sys_ConsoleInput(void)
{
	static char buf[256];
	int n;

	if(dedicated != nil && dedicated->value && iop >= 0){
		if(flen(pfd[1]) < 1)	/* only poll for input */
			return nil;
		if((n = read(pfd[1], buf, sizeof buf)) < 0)
			sysfatal("Sys_ConsoleInput:read: %r");
		if(n == 0){
			iop = -1;
			return nil;
		}
		return buf;
	}
	return nil;
}

void
IN_Grabm(int on)
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
		fixms++;
	}else if(fd >= 0){
		close(fd);
		fd = -1;
	}
}

void
IN_Commands(void)
{
	/* joystick stuff */
}

void
btnev(int btn, ulong msec)
{
	static int oldb;
	int i, b;

	for(i = 0; i < 3; i++){
		b = 1<<i;
		if(btn & b && ~oldb & b)
			Key_Event(K_MOUSE1+i, true, msec);
		else if(~btn & b && oldb & b)
			Key_Event(K_MOUSE1+i, false, msec);
	}
	oldb = btn & 7;
	/* mwheelup and mwheeldn buttons are never held down */
	for(i = 3; i < 5; i++){
		b = 1<<i;
		if(btn & b){
			Key_Event(K_MOUSE1+i, true, msec);
			Key_Event(K_MOUSE1+i, false, msec);
		}
	}
}

void
KBD_Update(void)
{
	int r;
	Kev ev;
	Mouse m;

	if(oldmwin != m_windowed->value){
		oldmwin = m_windowed->value;
		IN_Grabm(m_windowed->value);
	}
	while((r = nbrecv(kchan, &ev)) > 0)
		Key_Event(ev.key, ev.down, Sys_Milliseconds());
	if(r < 0)
		sysfatal("KBD_Update:nbrecv: %r\n");
	while((r = nbrecv(mchan, &m)) > 0){
		dx += m.xy.x;
		dy += m.xy.y;
		btnev(m.buttons, m.msec);
	}
	if(r < 0)
		sysfatal("KBD_Update:nbrecv: %r\n");
}

void
IN_Move(usercmd_t *cmd)
{
	static int oldmx, oldmy;
	int mx, my;

	if(!mouseon)
		return;

	if(m_filter->value){
		mx = (dx + oldmx) * 0.5;
		my = (dy + oldmy) * 0.5;
	}else{
		mx = dx;
		my = dy;
	}
	oldmx = dx;
	oldmy = dy;
	dx = dy = 0;
	if(!mx && !my)
		return;
	mx *= sensitivity->value;
	my *= sensitivity->value;

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
void
IN_Activate(qboolean)
{
}

/* called every frame even if not generating commands */
void
IN_Frame(void)
{
}

void
IN_ForceCenterView(void)
{
	cl.viewangles[PITCH] = 0;
}

void
IN_MLookDown(void)
{
	mlooking = true;
}

void
IN_MLookUp(void)
{
	mlooking = false;
	IN_CenterView();
}

static int
runetokey(Rune r)
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

static void
kproc(void *)
{
	int n, k, fd;
	char buf[128], kdown[128], *s;
	Rune r;
	Kev ev;

	if((fd = open("/dev/kbd", OREAD)) < 0)
		sysfatal("kproc: %r");
	kdown[0] = kdown[1] = buf[0] = 0;
	for(;;){
		if(buf[0] != 0){
			n = strlen(buf)+1;
			memmove(buf, buf+n, sizeof(buf)-n);
		}
		if(buf[0] == 0){
			n = read(fd, buf, sizeof(buf)-1);
			if(n <= 0)
				break;
			buf[n-1] = 0;
			buf[n] = 0;
		}
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
						if(send(kchan, &ev) < 0)
							goto end;
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
						if(send(kchan, &ev) < 0)
							goto end;
					}
				}
			}
			break;
		}
		strcpy(kdown, buf);
	}
end:;
}

static void
mproc(void *)
{
	int n, nerr, fd;
	char buf[1+5*12];
	Mouse m;
	Point p, o;

	if((fd = open("/dev/mouse", ORDWR)) < 0)
		sysfatal("mproc: %r");
	for(nerr=0;;){
		if((n = read(fd, buf, sizeof buf)) != 1+4*12){
			if(n < 0 || ++nerr > 10)
				break;
			fprint(2, "mproc: bad count %d not 49: %r\n", n);
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
			if(fixms){
				fixms = 0;
				goto res;
			}
			p.x = atoi(buf+1+0*12);
			p.y = atoi(buf+1+1*12);
			m.xy.x = p.x - o.x;
			m.xy.y = p.y - o.y;
			m.buttons = atoi(buf+1+2*12);
			m.msec = atoi(buf+1+3*12);
			if(nbsend(mchan, &m) < 0)
				goto end;
			if(!ptinrect(p, grabr)){
		res:
				fprint(fd, "m%d %d", center.x, center.y);
				p = center;
			}
			o = p;
			break;
		}
	}
end:;
}

static void
tproc(void *)	/* stupid select() timeout bullshit */
{
	int t, ms, n, r;

	threadsetgrp(THin);

	t = ms = 0;
	for(;;){
		sleep(1);
		t++;

		if((r = nbrecv(tchan, &n)) < 0)
			sysfatal("tproc:nbrecv: %r");
		if(r == 0){
			if(t == ms && nbsend(fuckchan, nil) < 0)
				sysfatal("tproc:nbsend: %r");
			continue;
		}
		if(n <= 0)
			ms = 0;
		else{
			ms = n;
			t = 0;
		}
	}
}

static void
iproc(void *)
{
	int n;
	char s[256];

	if((iop = pipe(pfd)) < 0)
		sysfatal("iproc:pipe: %r");
	for(;;){
		if((n = read(0, s, sizeof s)) <= 0)
			break;
		s[n-1] = 0;
		if((write(pfd[0], s, n)) != n)
			break;
		if(nbsend(fuckchan, nil) < 0)
			sysfatal("iproc:nbsend: %r");
	}
	fprint(2, "iproc %d: %r\n", threadpid(threadid()));
	iop = -1;
}

void
IN_Shutdown(void)
{
	IN_Grabm(0);
	iop = -1;
	close(pfd[0]);
	close(pfd[1]);
	if(kchan != nil)
		chanfree(kchan);
	if(mchan != nil)
		chanfree(mchan);
}

void
IN_Init(void)
{
	if(dedicated->value){
		if(proccreate(iproc, nil, 8192) < 0)
			sysfatal("proccreate iproc: %r");
		if(proccreate(tproc, nil, 8192) < 0)
			sysfatal("proccreate tproc: %r");
		return;
	}
	in_joystick = Cvar_Get("in_joystick", "0", CVAR_ARCHIVE);
	sensitivity = Cvar_Get("sensitivity", "3", CVAR_ARCHIVE);
	freelook = Cvar_Get("freelook", "0", CVAR_ARCHIVE);
	lookspring = Cvar_Get("lookspring", "0", CVAR_ARCHIVE);
	lookstrafe = Cvar_Get("lookstrafe", "0", CVAR_ARCHIVE);
	m_pitch = Cvar_Get("m_pitch", "0.022", CVAR_ARCHIVE);

	m_yaw = Cvar_Get("m_yaw", "0.022", 0);
	m_forward = Cvar_Get("m_forward", "1", 0);
	m_side = Cvar_Get("m_side", "0.8", 0);
	m_windowed = Cvar_Get("m_windowed", "0", CVAR_ARCHIVE);
	m_filter = Cvar_Get("m_filter", "0", 0);

	Cmd_AddCommand("+mlook", IN_MLookDown);
	Cmd_AddCommand("-mlook", IN_MLookUp);
	Cmd_AddCommand("force_centerview", IN_ForceCenterView);

	if((kchan = chancreate(sizeof(Kev), Nbuf)) == nil)
		sysfatal("chancreate kchan: %r");
	if(proccreate(kproc, nil, 8192) < 0)
		sysfatal("proccreate kproc: %r");
	if((mchan = chancreate(sizeof(Mouse), Nbuf)) == nil)
		sysfatal("chancreate kchan: %r");
	if(proccreate(mproc, nil, 8192) < 0)
		sysfatal("proccreate mproc: %r");
}
