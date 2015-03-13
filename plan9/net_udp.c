#include <u.h>
#include <libc.h>
#include <stdio.h>
#include <ctype.h>
#include <bio.h>
#include <ndb.h>
#include "../dat.h"
#include "../fns.h"

/* FIXME: only loopback works, the rest is complete bullshit */

typedef struct Loopmsg Loopmsg;
typedef struct Loopback Loopback;

enum{
	LOOPBACK	= 0x7f000001,
	MAX_LOOPBACK	= 4
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

int	ipfd[2], ipxfd[2];
netadr_t laddr;


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
	static char s[64];
	
	Com_sprintf(s, sizeof(s), "%i.%i.%i.%i:%hi", a.ip[0], a.ip[1], a.ip[2], a.ip[3], a.port << 8 | a.port >> 8);
	return s;
}

char *NET_BaseAdrToString (netadr_t a)
{
	static char s[64];
	
	Com_sprintf(s, sizeof(s), "%i.%i.%i.%i", a.ip[0], a.ip[1], a.ip[2], a.ip[3]);
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
qboolean NET_StringToAdr (char *s, netadr_t *a)
{
	int i;
	char *e, *p, *ss;
	Ndb *db;
	Ndbtuple *nt;

	if(!strcmp(s, "localhost")){
		memset(a, 0, sizeof(*a));
		a->type = NA_LOOPBACK;
		return true;
	}

	/* FIXMEGASHIT */
	if((ss = smprint("%s", s)) == nil)		/* don't fuck with someone else's s */
		sysfatal("NET_StringToAdr:smprint: %r");
	p = ss;
	if((e = strchr(p, ':')) != nil){
		*e++ = '\0';
		a->port = atoi(e);
	}
	db = ndbopen(nil);
	if((nt = ndbgetipaddr(db, ss)) == nil){
		ndbclose(db);
		return false;
	}
	if((ss = smprint("%s", nt->val)) == nil)	/* get copy of first value found */
		sysfatal("NET_StringToAdr:smprint: %r");
	p = ss;
	for(i = 0; i < 4; i++){
		if((e = strchr(p, '.')) != nil)
			*e++ = '\0';
		a->ip[i] = atoi(p);
		p = e;
	}
	a->type = NA_IP;

	free(ss);
	ndbfree(nt);
	ndbclose(db);
	return true;
}

qboolean NET_IsLocalAddress (netadr_t adr)
{
	return NET_CompareAdr (adr, laddr);
}

qboolean NET_GetLoopPacket (netsrc_t sock, netadr_t *net_from, sizebuf_t *net_message)
{
	int i;
	Loopback *loop;

	loop = &loopbacks[sock];

	if(loop->send - loop->get > MAX_LOOPBACK)
		loop->get = loop->send - MAX_LOOPBACK;

	if (loop->get >= loop->send)
		return false;

	i = loop->get & (MAX_LOOPBACK-1);
	loop->get++;

	memcpy(net_message->data, loop->msgs[i].data, loop->msgs[i].datalen);
	net_message->cursize = loop->msgs[i].datalen;
	*net_from = laddr;
	return true;
}

void NET_SendLoopPacket (netsrc_t sock, int length, void *data, netadr_t /*to*/)
{
	Loopback *loop;
	int i;

	loop = &loopbacks[sock^1];
	i = loop->send & (MAX_LOOPBACK-1);
	loop->send++;
	memcpy(loop->msgs[i].data, data, length);
	loop->msgs[i].datalen = length;
}

qboolean NET_GetPacket (netsrc_t src, netadr_t *from, sizebuf_t *d)
{
	int n, protocol, fd;
	NetConnInfo *nci;
	char addr[21];

	if(NET_GetLoopPacket(src, from, d))
		return true;

	for(protocol = 0 ; protocol < 2 ; protocol++){
		if(protocol == 0)
			fd = ipfd[src];
		else
			fd = ipxfd[src];
		if(!fd)
			continue;

		if((n = read(fd, d->data, d->maxsize)) < 0){
			fprint(2, "NET_GetPacket:read: %r\n");
			Com_Printf("NET_GetPacket: error reading packet\n");
			continue;
		}
		if(n == d->maxsize){
			Com_Printf("Oversize packet from %s\n", NET_AdrToString(*from));
			continue;
		}
		d->cursize = n;

		/* FIXME */
		if((nci = getnetconninfo(nil, fd)) == nil){
			fprint(2, "NET_GetPacket:getnetconninfo: %r\n");
			return true;
		}
		seprint(addr, addr+sizeof(addr), "%s:%s", nci->rsys, nci->rserv);
		NET_StringToAdr(addr, from);
		return true;
	}
	return false;
}

void NET_SendPacket (netsrc_t src, int length, void *data, netadr_t to)
{
	int fd = 0;

	switch(to.type){
	case NA_LOOPBACK:
		NET_SendLoopPacket(src, length, data, to);
		return;
	case NA_BROADCAST:
	case NA_IP:
		fd = ipfd[src];
		if(!fd)
			return;
		break;
	case NA_IPX:
	case NA_BROADCAST_IPX:
		fd = ipxfd[src];
		if(!fd)
			return;
		break;
	default:
		Com_Error(ERR_FATAL, "NET_SendPacket: bad address type");
	}
	if(write(fd, data, length) != length){
		fprint(2, "NET_SendPacket:write: %r\n");
		Com_Printf("NET_SendPacket: bad write\n");
	}
}

int NET_Socket (char *ifc, int port)
{
	int fd;
	char addr[21], dir[40], *p;

	/* FIXMEGAFUCKEDUPSHIT */
	p = seprint(addr, addr+sizeof(addr), "udp!*");
	if(ifc != nil || Q_strcasecmp(ifc, "localhost"))
		p = seprint(p-1, addr+sizeof(addr), "%s", ifc);
	if(port == PORT_ANY)
		seprint(p, addr+sizeof(addr), "!%hud", (ushort)port);

	if((fd = announce(addr, dir)) < 0){
		fprint(2, "NET_Socket:announce: %r\n");
		Com_Printf("Net_Socket: announce failed!\n");
		return false;
	}
	return fd;	/* FIXME: NO! */
}

void NET_OpenIP (void)
{
	cvar_t	*port, *ip;

	port = Cvar_Get("port", va("%i", PORT_SERVER), CVAR_NOSET);
	ip = Cvar_Get("ip", "localhost", CVAR_NOSET);

	/* FIXME: those are ctl fd's! */
	if(!ipfd[NS_SERVER])
		ipfd[NS_SERVER] = NET_Socket(ip->string, port->value);
	if(!ipfd[NS_CLIENT])
		ipfd[NS_CLIENT] = NET_Socket(ip->string, PORT_ANY);
}

void NET_OpenIPX (void)
{
}

/* a single player game will only use the loopback code */
void NET_Config (qboolean multiplayer)
{
	int i;

	if(!multiplayer){	/* shut down any existing sockets */
		for(i=0 ; i<2 ; i++){
			if(ipfd[i]){
				close(ipfd[i]);
				ipfd[i] = 0;
			}
			if(ipxfd[i]){
				close(ipxfd[i]);
				ipxfd[i] = 0;
			}
		}
	}
	else{			/* open sockets */
		NET_OpenIP();
		NET_OpenIPX();
	}
}

void NET_Init (void)
{
}

void NET_Shutdown (void)
{
	NET_Config(false);	// close sockets
}

/* sleeps msec or until net socket is ready */
void NET_Sleep(int /*msec*/)
{
	/* PORTME */

	/*
    struct timeval timeout;
	fd_set	fdset;
	extern cvar_t *dedicated;
	extern qboolean stdin_active;
	*/

	if(!ipfd[NS_SERVER] || dedicated && !dedicated->value)
		return; // we're not a server, just run full speed

	/*
	FD_ZERO(&fdset);
	if (stdin_active)
		FD_SET(0, &fdset); // stdin is processed too
	FD_SET(ipfd[NS_SERVER], &fdset); // network socket
	timeout.tv_sec = msec/1000;
	timeout.tv_usec = (msec%1000)*1000;
	select(ipfd[NS_SERVER]+1, &fdset, NULL, NULL, &timeout);
	*/
}
