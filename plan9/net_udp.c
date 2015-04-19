#include <u.h>
#include <libc.h>
#include <stdio.h>
#include <thread.h>
#include <bio.h>
#include <ndb.h>
#include <ip.h>
#include "../q_shared.h"

/* FIXME: this shit SUCKS, and ipv4 only because of other code */

cvar_t	*svport;	/* server port and copy of string value */
char	srv[6];
cvar_t	*clport;	/* "client" port and copy */
char	clsrv[6];

typedef struct Loopmsg Loopmsg;
typedef struct Loopback Loopback;
typedef struct Conmsg Conmsg;
typedef struct Conlist Conlist;

enum{
	MAX_LOOPBACK	= 4,
	HDRSZ		= 16+16+16+2+2,	/* sizeof Udphdr w/o padding */
	BUFSZ		= MAX_MSGLEN,
	NBUF		= 64,
	DTHGRP		= 1,
	CLPORT		= 27909
};
struct Loopmsg{
	byte	data[MAX_MSGLEN];
	int	datalen;
};
struct Loopback{
	Loopmsg	msgs[MAX_LOOPBACK];
	int	get;
	int	send;
};
Loopback	loopbacks[2];

struct Conlist{
	Conlist *p;
	uchar	u[IPaddrlen+2];
	char	addr[IPaddrlen*2+8+6];	/* ipv6 + separators + port in decimal */
	int	dfd;
	Udphdr	h;
	int	src;	/* q2 assumes broadcast replies are received on NS_CLIENT */
};
Conlist *cnroot;

struct Conmsg{
	Conlist *p;
	int	n;
	uchar	buf[BUFSZ];
};
Channel *udpchan;
Channel *clchan;

netadr_t laddr;		/* 0.0.0.0:0 */
int cfd = -1, ufd = -1, clfd = -1, cldfd = -1;
QLock cnlock;


qboolean NET_CompareAdr (netadr_t a, netadr_t b)
{
	return (a.ip[0] == b.ip[0] && a.ip[1] == b.ip[1] && a.ip[2] == b.ip[2] && a.ip[3] == b.ip[3] && a.port == b.port);
}

/* compares without the port */
qboolean NET_CompareBaseAdr (netadr_t a, netadr_t b)
{
	if(a.type != b.type)
		return false;
	switch(a.type){
	case NA_LOOPBACK:
		return true;
	case NA_IP:
		return (a.ip[0] == b.ip[0] && a.ip[1] == b.ip[1] && a.ip[2] == b.ip[2] && a.ip[3] == b.ip[3]);
	case NA_IPX:
		return !memcmp(a.ipx, b.ipx, 10);
	default:
		return false;
	}
}

char *NET_AdrToString (netadr_t a)
{
	static char s[256];

	seprint(s, s+sizeof s, "%ud.%ud.%ud.%ud:%hud", a.ip[0], a.ip[1], a.ip[2], a.ip[3], BigShort(a.port));
	return s;
}

char *NET_BaseAdrToString (netadr_t a)
{
	static char s[256];

	seprint(s, s+sizeof s, "%ud.%ud.%ud.%ud", a.ip[0], a.ip[1], a.ip[2], a.ip[3]);
	return s;
}

/*
=============
NET_StringToAdr

localhost
idnewt
idnewt:28000
192.246.40.70
192.246.40.70:28000
=============
*/
qboolean NET_StringToAdr (char *addr, netadr_t *a)		/* assumes IPv4 */
{
	int i;
	char s[256], *p, *pp;
	Ndb *db;
	Ndbtuple *nt;

	if(!strcmp(addr, "localhost")){
		memset(a, 0, sizeof *a);
		a->type = NA_LOOPBACK;
		return true;
	}

	strncpy(s, addr, sizeof s);
	s[sizeof(s)-1] = 0;

	if((p = strrchr(s, ':')) != nil){
		*p++ = '\0';
		a->port = BigShort(atoi(p));
	}

	if((db = ndbopen(nil)) == nil){
		fprint(2, "NET_StringToAdr:ndbopen: %r\n");
		return false;
	}
	if((nt = ndbgetipaddr(db, s)) == nil){
		ndbclose(db);
		fprint(2, "NET_StringToAdr:ndbgetipaddr: %r\n");
		return false;
	}
	strncpy(s, nt->val, sizeof(s)-1);	/* just look at first value found */
	ndbfree(nt);
	ndbclose(db);

	for(i = 0, pp = s; i < IPv4addrlen; i++){
		if((p = strchr(pp, '.')) != nil)
			*p++ = '\0';
		a->ip[i] = atoi(pp);
		pp = p;
	}
	a->type = NA_IP;
	return true;
}

qboolean NET_IsLocalAddress (netadr_t adr)
{
	return NET_CompareAdr(adr, laddr);
}

qboolean looprecv (netsrc_t sock, netadr_t *net_from, sizebuf_t *d)
{
	int i;
	Loopback *l;

	l = &loopbacks[sock];
	if(l->send - l->get > MAX_LOOPBACK)
		l->get = l->send - MAX_LOOPBACK;
	if(l->get >= l->send)
		return false;
	i = l->get & (MAX_LOOPBACK-1);
	l->get++;

	memcpy(d->data, l->msgs[i].data, l->msgs[i].datalen);
	d->cursize = l->msgs[i].datalen;
	*net_from = laddr;
	return true;
}

void loopsend (netsrc_t sock, int length, void *data, netadr_t /*to*/)
{
	Loopback *l;
	int i;

	l = &loopbacks[sock^1];
	i = l->send & (MAX_LOOPBACK-1);
	l->send++;
	memcpy(l->msgs[i].data, data, length);
	l->msgs[i].datalen = length;
}

void cninit (void)
{
	if(cnroot != nil)
		return;
	if((cnroot = malloc(sizeof *cnroot)) == nil)
		sysfatal("cninit:malloc: %r");
	cnroot->p = cnroot;
	memset(cnroot->u, 0, sizeof cnroot->u);
	memset(cnroot->addr, 0, sizeof cnroot->addr);
	cnroot->dfd = -1;
}

Conlist *cnins (int fd, char *addr, uchar *u, Udphdr *h, int src)
{
	Conlist *p, *l;

	l = cnroot;
	if((p = malloc(sizeof *p)) == nil)
		sysfatal("cnins:malloc: %r");

	strncpy(p->addr, addr, sizeof p->addr);
	memcpy(p->u, u, sizeof p->u);
	p->dfd = fd;
	if(h != nil)
		memcpy(&p->h, h, sizeof p->h);
	p->src = src;
	p->p = l->p;
	l->p = p;
	return p;
}

Conlist *cnfind (char *raddr)
{
	Conlist *p = cnroot->p;

	while(p != cnroot){
		if(!strncmp(p->addr, raddr, strlen(p->addr)))
			return p;
		p = p->p;
	}
	return nil;
}

void cndel (Conlist *p)
{
	Conlist *l = cnroot;

	while(l->p != p){
		l = l->p;
		if(l == cnroot)
			sysfatal("cndel: bad unlink: cnroot 0x%p node 0x%p\n", cnroot, p);
	}
	l->p = p->p;
	if(p->dfd != ufd && p->dfd != cldfd && p->dfd != -1)
		close(p->dfd);
	free(p);
}

void cnnuke (void)
{
	Conlist *p, *l = cnroot;

	if(cnroot == nil)
		return;
	do{
		p = l;
		l = l->p;
		if(p->dfd != -1)
			close(p->dfd);
		free(p);
	}while(l != cnroot);
	cnroot = nil;
}

void dproc (void *me)
{
	int n, fd;
	Conmsg m;
	Conlist *p;
	Channel *c;

	if(threadsetgrp(DTHGRP) < 0)
		sysfatal("dproc:threadsetgrp: %r");

	m.p = p = me;
	c = p->src == NS_CLIENT ? clchan : udpchan;
	fd = p->dfd;

	for(;;){
		if((n = read(fd, m.buf, sizeof m.buf)) <= 0)
			break;
		m.n = n;
		if(send(c, &m) < 0)
			sysfatal("uproc:send: %r\n");
	}
	fprint(2, "dproc %d: %r\n", threadpid(threadid()));
	cndel(me);
}

void uproc (void *c)
{
	int n, fd;
	uchar udpbuf[BUFSZ+HDRSZ], u[IPaddrlen+2];
	char a[IPaddrlen*2+8+6];
	Udphdr h;
	Conmsg m;
	Conlist *p;

	if(threadsetgrp(DTHGRP) < 0)
		sysfatal("uproc:threadsetgrp: %r");

	fd = ufd;
	if(c == clchan)
		fd = cldfd;
	for(;;){
		if((n = read(fd, udpbuf, sizeof udpbuf)) <= 0)
			sysfatal("uproc:read: %r\n");
		memcpy(&h, udpbuf, HDRSZ);

		memcpy(u, h.raddr, IPaddrlen);
		memcpy(u+IPaddrlen, h.rport, 2);
		snprint(a, sizeof a, "%ud.%ud.%ud.%ud:%hud", u[12], u[13], u[14], u[15], u[16]<<8 | u[17]);
		qlock(&cnlock);
		if((p = cnfind(a)) == nil)
			p = cnins(fd, a, u, &h, 0);
		qunlock(&cnlock);
		m.p = p;

		if(n - HDRSZ < 0){	/* FIXME */
			m.n = n;
			memcpy(m.buf, udpbuf, m.n);
		}else{
			m.n = n - HDRSZ;
			memcpy(m.buf, udpbuf+HDRSZ, m.n);
		}
		if(send(c, &m) < 0)
			sysfatal("uproc:send: %r\n");
	}
}

qboolean NET_GetPacket (netsrc_t src, netadr_t *from, sizebuf_t *d)
{
	int n;
	Conmsg m;

	if(looprecv(src, from, d))
		return true;
	if(cfd == -1)
		return false;

	if((n = nbrecv(src == NS_SERVER ? udpchan : clchan, &m)) < 0)
		sysfatal("NET_GetPacket:nbrecv: %r");
	if(n == 0)
		return false;

	memcpy(from->ip, m.p->u+12, 4);
	from->port = m.p->u[17] << 8 | m.p->u[16];
	if(m.n == d->maxsize){
		Com_Printf("Oversize packet from %s\n", NET_AdrToString(*from));
		return false;
	}
	from->type = NA_IP;
	d->cursize = m.n;
	memcpy(d->data, m.buf, m.n);
	return true;
}

void NET_SendPacket (netsrc_t src, int length, void *data, netadr_t to)
{
	int fd;
	char *addr, *s, *lport;
	uchar b[BUFSZ+HDRSZ], u[IPaddrlen+2];
	Conlist *p;

	switch(to.type){
	case NA_LOOPBACK:
		loopsend(src, length, data, to);
		break;
	case NA_BROADCAST_IPX:
	case NA_IPX:
		break;
	case NA_BROADCAST:
		memset(to.ip, 0xff, 4);
		addr = NET_AdrToString(to);	/* port is PORT_SERVER */
		s = strrchr(addr, ':');
		*s++ = '\0';
		if((fd = dial(netmkaddr(addr, "udp", s), clsrv, nil, nil)) < 0)
			sysfatal("NET_SendPacket:dial bcast: %r");
		if(write(fd, data, length) != length)
			sysfatal("NET_SendPacket:write bcast: %r");
		close(fd);
		break;
	case NA_IP:
		if(cfd == -1)
			break;

		addr = NET_AdrToString(to);
		qlock(&cnlock);
		p = cnfind(addr);
		qunlock(&cnlock);
		if(p != nil){
			fd = p->dfd;
			if(fd == ufd || fd == cldfd){
				memcpy(b, &p->h, HDRSZ);
				memcpy(b+HDRSZ, data, length);
				write(fd, b, length+HDRSZ);
				break;
			}
		}else{
			lport = strrchr(addr, ':');
			*lport++ = '\0';
			s = netmkaddr(addr, "udp", lport);
			if((fd = dial(s, srv, nil, nil)) < 0)
				sysfatal("NET_SendPacket:dial: %r");

			memcpy(u, v4prefix, sizeof v4prefix);
			memcpy(u+IPv4off, to.ip, IPv4addrlen);
			u[16] = to.port;
			u[17] = to.port >> 8;
			*(lport-1) = ':';
			qlock(&cnlock);
			p = cnins(fd, addr, u, nil, src);
			qunlock(&cnlock);

			if(proccreate(dproc, p, 8196) < 0)
				sysfatal("NET_SendPacket:proccreate: %r");
		}
		if(write(fd, data, length) != length)
			sysfatal("NET_SendPacket:write: %r");
		break;
	default:
		Com_Error(ERR_FATAL, "NET_SendPacket: bad address type");
	}
}

/* sleeps msec or until data is read from dfd */
void NET_Sleep (int msec)
{
	if(cfd == -1 || dedicated != nil && !dedicated->value)
		return; // we're not a server, just run full speed

	/* FIXME */
	print("NET_Sleep %d: PORTME\n", msec);
}

int openname (char *port, int *dfd, Channel **c)
{
	int fd;
	char data[64], adir[40];

	if((fd = announce(netmkaddr("*", "udp", port), adir)) < 0)
		sysfatal("openname:announce udp!*!%s: %r", port);
	if(fprint(fd, "headers") < 0)
		sysfatal("openname: failed to set header mode: %r");
	snprint(data, sizeof data, "%s/data", adir);
	if((*dfd = open(data, ORDWR)) < 0)
		sysfatal("openname:open %r");
	if((*c = chancreate(sizeof(Conmsg), NBUF)) == nil)
		sysfatal("openname:chancreate: %r");
	if(proccreate(uproc, *c, 8196) < 0)
		sysfatal("openname:proccreate: %r");
	return fd;
}

void openudp (void)
{
	if(cfd != -1)
		return;

	/* svport value can be changed at any time */
	if(svport->value == PORT_ANY || svport->value > 32767)
		/* FIXME */
		strncpy(srv, "*", sizeof(srv)-1);
	else
		strncpy(srv, svport->string, sizeof(srv)-1);
	cfd = openname(srv, &ufd, &udpchan);

	/* broadcast kluge */
	if(clport->value == PORT_ANY || clport->value > 32767)
		strncpy(clsrv, "*", sizeof(clsrv)-1);
	else
		strncpy(clsrv, clport->string, sizeof(clsrv)-1);
	clfd = openname(clsrv, &cldfd, &clchan);
}

/* a single player game will only use the loopback code */
void NET_Config (qboolean multiplayer)
{
	if(!multiplayer){	/* shut down existing udp connections */
		threadkillgrp(DTHGRP);
		cnnuke();
		if(udpchan != nil){
			chanfree(udpchan);
			udpchan = nil;
		}
		if(clchan != nil){
			chanfree(clchan);
			clchan = nil;
		}
		if(cfd != -1){
			close(cfd);
			cfd = -1;
		}
		if(clfd != -1){
			close(clfd);
			clfd = -1;
		}
		if(ufd != -1){
			close(ufd);
			ufd = -1;
		}
		if(cldfd != -1){
			close(cldfd);
			cldfd = -1;
		}
	}else{			/* announce open line and get cfd for it */
		cninit();
		openudp();
	}
}

void NET_Shutdown (void)
{
	NET_Config(false);
}

void NET_Init (void)
{
	svport = Cvar_Get("port", va("%d", PORT_SERVER), CVAR_NOSET);
	clport = Cvar_Get("clport", va("%hud", CLPORT), CVAR_NOSET);
}
