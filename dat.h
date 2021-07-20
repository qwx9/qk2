//#define PARANOID	// speed sapping error checking
//#define SMALL_FINALVERT

typedef uchar byte;
typedef float vec_t;
typedef vec_t vec3_t[3];
typedef vec_t vec5_t[5];
typedef	int fixed4_t;
typedef	int fixed8_t;
typedef	int fixed16_t;
typedef uchar pixel_t;

typedef enum qboolean {false, true} qboolean;

typedef struct edict_t edict_t;
typedef struct cvar_t cvar_t;
typedef struct cplane_t cplane_t;
typedef struct cmodel_t cmodel_t;
typedef struct csurface_t csurface_t;
typedef struct mapsurface_t mapsurface_t;
typedef struct trace_t trace_t;
typedef struct pmove_state_t pmove_state_t;
typedef struct usercmd_t usercmd_t;
typedef struct pmove_t pmove_t;
typedef struct entity_state_t entity_state_t;
typedef struct player_state_t player_state_t;
typedef struct sizebuf_t sizebuf_t;
typedef struct netadr_t netadr_t;
typedef struct netchan_t netchan_t;
typedef struct dpackfile_t dpackfile_t;
typedef struct dpackheader_t dpackheader_t;
typedef struct pcx_t pcx_t;
typedef struct dstvert_t dstvert_t;
typedef struct dtriangle_t dtriangle_t;
typedef struct dtrivertx_t dtrivertx_t;
typedef struct daliasframe_t daliasframe_t;
typedef struct dmdl_t dmdl_t;
typedef struct dsprframe_t dsprframe_t;
typedef struct dsprite_t dsprite_t;
typedef struct miptex_t miptex_t;
typedef struct lump_t lump_t;
typedef struct dheader_t dheader_t;
typedef struct dmodel_t dmodel_t;
typedef struct dvertex_t dvertex_t;
typedef struct dplane_t dplane_t;
typedef struct dnode_t dnode_t;
typedef struct texinfo_t texinfo_t;
typedef struct dedge_t dedge_t;
typedef struct dface_t dface_t;
typedef struct dleaf_t dleaf_t;
typedef struct dbrushside_t dbrushside_t;
typedef struct dbrush_t dbrush_t;
typedef struct dvis_t dvis_t;
typedef struct dareaportal_t dareaportal_t;
typedef struct darea_t darea_t;
typedef struct vrect_t vrect_t;
typedef struct viddef_t viddef_t;
typedef struct image_t image_t;
typedef struct oldrefdef_t oldrefdef_t;
typedef struct mvertex_t mvertex_t;
typedef struct mplane_t mplane_t;
typedef struct medge_t medge_t;
typedef struct mtexinfo_t mtexinfo_t;
typedef struct msurface_t msurface_t;
typedef struct mnode_t mnode_t;
typedef struct mleaf_t mleaf_t;
typedef struct model_t model_t;
typedef struct emitpoint_t emitpoint_t;
typedef struct finalvert_t finalvert_t;
typedef struct affinetridesc_t affinetridesc_t;
typedef struct drawsurf_t drawsurf_t;
typedef struct alight_t alight_t;
typedef struct bedge_t bedge_t;
typedef struct clipplane_t clipplane_t;
typedef struct surfcache_t surfcache_t;
typedef struct espan_t espan_t;
typedef struct polydesc_t polydesc_t;
typedef struct surf_t surf_t;
typedef struct edge_t edge_t;
typedef struct aliastriangleparms_t aliastriangleparms_t;
typedef struct swstate_t swstate_t;
typedef struct entity_t entity_t;
typedef struct dlight_t dlight_t;
typedef struct particle_t particle_t;
typedef struct lightstyle_t lightstyle_t;
typedef struct refdef_t refdef_t;
typedef struct refexport_t refexport_t;
typedef struct refimport_t refimport_t;
typedef struct portable_samplepair_t portable_samplepair_t;
typedef struct sfxcache_t sfxcache_t;
typedef struct sfx_t sfx_t;
typedef struct playsound_t playsound_t;
typedef struct dma_t dma_t;
typedef struct channel_t channel_t;
typedef struct wavinfo_t wavinfo_t;
typedef struct console_t console_t;
typedef struct frame_t frame_t;
typedef struct centity_t centity_t;
typedef struct clientinfo_t clientinfo_t;
typedef struct client_state_t client_state_t;
typedef struct client_static_t client_static_t;
typedef struct cdlight_t cdlight_t;
typedef struct cl_sustain_t cl_sustain_t;
typedef struct cparticle_t cparticle_t;
typedef struct kbutton_t kbutton_t;
typedef struct menuframework_t menuframework_t;
typedef struct menucommon_t menucommon_t;
typedef struct menufield_t menufield_t;
typedef struct menuslider_t menuslider_t;
typedef struct menulist_t menulist_t;
typedef struct menuaction_t menuaction_t;
typedef struct menuseparator_t menuseparator_t;
typedef struct server_t server_t;
typedef struct client_frame_t client_frame_t;
typedef struct client_t client_t;
typedef struct challenge_t challenge_t;
typedef struct server_static_t server_static_t;

typedef void	(*xcommand_t)(void);
typedef	refexport_t	(*GetRefAPI_t)(refimport_t);

enum{
	/* angle indexes */
	PITCH = 0,
	YAW = 1,
	ROLL = 2,

	MAX_STRING_CHARS = 1024,	// char* passed to Cmd_TokenizeString, max strlen
	MAX_STRING_TOKENS = 80,	// max tokens resulting from Cmd_TokenizeString
	MAX_TOKEN_CHARS = 128,	// max length of an individual token

	/* max length of quake game and fs pathname */
	MAX_QPATH = 64,
	MAX_OSPATH = 128,

	/* per-level limits */
	MAX_CLIENTS = 256,	// absolute limit
	MAX_EDICTS = 1024,	// must change protocol to increase more
	MAX_LIGHTSTYLES = 256,

	/* these are sent over the net as bytes so they cannot be blindly increased */
	MAX_MODELS = 256,
	MAX_SOUNDS = 256,
	MAX_IMAGES = 256,
	MAX_ITEMS = 256,

	MAX_GENERAL = MAX_CLIENTS*2,	// general config strings

	/* game print flags */
	PRINT_LOW = 0,		// pickup messages
	PRINT_MEDIUM = 1,	// death messages
	PRINT_HIGH = 2,		// critical messages
	PRINT_CHAT = 3,		// chat messages

	/* key / value info strings */
	MAX_INFO_KEY = 64,
	MAX_INFO_VALUE = 64,
	MAX_INFO_STRING = 512,

	MAX_PARSE_ENTITIES = 1024,
};

/* destination class for gi.multicast() */
typedef enum multicast_t{
	MULTICAST_ALL,
	MULTICAST_PHS,
	MULTICAST_PVS,
	MULTICAST_ALL_R,
	MULTICAST_PHS_R,
	MULTICAST_PVS_R
}multicast_t;

#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h

extern vec3_t vec3_origin;

#define DotProduct(x,y)	(x[0]*y[0]+x[1]*y[1]+x[2]*y[2])
#define VectorSubtract(a,b,c)	(c[0]=a[0]-b[0],c[1]=a[1]-b[1],c[2]=a[2]-b[2])
#define VectorAdd(a,b,c)	(c[0]=a[0]+b[0],c[1]=a[1]+b[1],c[2]=a[2]+b[2])
#define VectorCopy(a,b)	(b[0]=a[0],b[1]=a[1],b[2]=a[2])
#define VectorClear(a)	(a[0]=a[1]=a[2]=0)
#define VectorNegate(a,b)	(b[0]=-a[0],b[1]=-a[1],b[2]=-a[2])
#define VectorSet(v, x, y, z)	(v[0]=(x), v[1]=(y), v[2]=(z))
#define BOX_ON_PLANE_SIDE(emins, emaxs, p)	\
	(((p)->type < 3)?						\
	(										\
		((p)->dist <= (emins)[(p)->type])?	\
			1								\
		:									\
		(									\
			((p)->dist >= (emaxs)[(p)->type])?\
				2							\
			:								\
				3							\
		)									\
	)										\
	:										\
		BoxOnPlaneSide( (emins), (emaxs), (p)))

extern int curtime;	// current time in ms, from Sys_Milliseconds()

enum{
	CVAR_ARCHIVE = 1<<0,	// save to vars.rc
	CVAR_USERINFO = 1<<1,	// add to userinfo on change
	CVAR_SERVERINFO = 1<<2,	// add to serverinfo on change
	CVAR_NOSET = 1<<3,	// only allow setting from commandline
	CVAR_LATCH= 1<<4	// save changes until server restart
};
/* nothing outside the Cvar_*() functions should modify these fields! */
struct cvar_t{
	char *name;
	char *string;
	char *latched_string;	// for CVAR_LATCH vars
	int flags;
	qboolean modified;	// set each time the cvar is changed
	float value;
	cvar_t *next;
};
extern cvar_t *cvar_vars;
extern qboolean userinfo_modified;

enum{
	/* contents flags are separate bits. a given brush can contribute
	 * multiple content bits. multiple brushes can be in a single leaf
	 * lower bits are stronger, and will eat weaker brushes completely */
	CONTENTS_SOLID = 1<<0,	// an eye is never valid in a solid
	CONTENTS_WINDOW = 1<<1,	// translucent, but not watery
	CONTENTS_AUX = 1<<2,
	CONTENTS_LAVA = 1<<3,
	CONTENTS_SLIME = 1<<4,
	CONTENTS_WATER = 1<<5,
	CONTENTS_MIST = 1<<6,
	LAST_VISIBLE_CONTENTS = 1<<6,
	/* remaining contents are non-visible, and don't eat brushes */
	CONTENTS_AREAPORTAL = 1<<15,
	CONTENTS_PLAYERCLIP = 1<<16,
	CONTENTS_MONSTERCLIP = 1<<17,
	/* currents can be added to any other contents, and may be mixed */
	CONTENTS_CURRENT_0 = 1<<18,
	CONTENTS_CURRENT_90 = 1<<19,
	CONTENTS_CURRENT_180 = 1<<20,
	CONTENTS_CURRENT_270 = 1<<21,
	CONTENTS_CURRENT_UP = 1<<22,
	CONTENTS_CURRENT_DOWN = 1<<23,
	CONTENTS_ORIGIN = 1<<24,	// removed before bsping an entity
	CONTENTS_MONSTER = 1<<25,	// should never be on a brush, only in game
	CONTENTS_DEADMONSTER = 1<<26,
	CONTENTS_DETAIL = 1<<27,	// brushes to be added after vis leafs
	CONTENTS_TRANSLUCENT = 1<<28,	// auto set if any surface has trans
	CONTENTS_LADDER = 1<<29,

	SURF_LIGHT = 1<<0,	// value will hold the light strength
	SURF_SLICK = 1<<1,	// effects game physics
	SURF_SKY = 1<<2,	// don't draw, but add to skybox
	SURF_WARP = 1<<3,	// turbulent water warp
	SURF_TRANS33 = 1<<4,
	SURF_TRANS66 = 1<<5,
	SURF_FLOWING = 1<<6,	// scroll towards angle
	SURF_NODRAW = 1<<7,	// don't bother referencing the texture

	/* content masks */
	MASK_ALL = -1,
	MASK_SOLID = CONTENTS_SOLID|CONTENTS_WINDOW,
	MASK_PLAYERSOLID = CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER,
	MASK_DEADSOLID = CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW,
	MASK_MONSTERSOLID = CONTENTS_SOLID|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER,
	MASK_WATER = CONTENTS_WATER|CONTENTS_LAVA|CONTENTS_SLIME,
	MASK_OPAQUE = CONTENTS_SOLID|CONTENTS_SLIME|CONTENTS_LAVA,
	MASK_SHOT = CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEADMONSTER,
	MASK_CURRENT = CONTENTS_CURRENT_0|CONTENTS_CURRENT_90|CONTENTS_CURRENT_180|CONTENTS_CURRENT_270|CONTENTS_CURRENT_UP|CONTENTS_CURRENT_DOWN,

	// FIXME: eliminate AREA_ distinction?
	/* SV_AreaEdicts() can return a list of either solid or trigger entities */
	AREA_SOLID = 1<<0,
	AREA_TRIGGERS = 1<<1
};

/* !!! if this is changed, it must be changed in asm code too !!! */
struct cplane_t{
	vec3_t normal;
	float dist;
	uchar type;	// for fast side tests
	uchar signbits;	// signx + (signy<<1) + (signz<<1)
	uchar pad[2];
};
struct cmodel_t{
	vec3_t mins;
	vec3_t maxs;
	vec3_t origin;	// for sounds or lights
	int headnode;
};
struct csurface_t{
	char name[16];
	int flags;
	int value;
};
/* used internally due to name len probs */
struct mapsurface_t{
	csurface_t c;
	char rname[32];
};
/* a trace is returned when a box is swept through the world */
struct trace_t{
	qboolean allsolid;	// if true, plane is not valid
	qboolean startsolid;	// if true, the initial point was in a solid area
	float fraction;	// time completed, 1.0 = didn't hit anything
	vec3_t endpos;	// final position
	cplane_t plane;	// surface normal at impact
	csurface_t *surface;	// surface hit
	int contents;	// contents on other side of surface hit
	edict_t	*ent;	// not set by CM_*() functions
};

/* information necessary for client side movement prediction */
typedef enum pmtype_t{
	/* can accelerate and turn */
	PM_NORMAL,
	PM_SPECTATOR,
	/* no acceleration or turning */
	PM_DEAD,
	PM_GIB,	// different bounding box
	PM_FREEZE
}pmtype_t;

enum{
	PMF_DUCKED = 1<<0,
	PMF_JUMP_HELD = 1<<1,
	PMF_ON_GROUND = 1<<2,
	PMF_TIME_WATERJUMP = 1<<3,	// pm_time is waterjump
	PMF_TIME_LAND = 1<<4,	// pm_time is time before rejump
	PMF_TIME_TELEPORT = 1<<5,	// pm_time is non-moving time
	/* temporarily disables prediction (used for grappling hook) */
	PMF_NO_PREDICTION = 1<<6,
};
/* this structure needs to be communicated bit-accurate from the server to the
 * client to guarantee that prediction stays in sync, so no floats are used.
 * if any part of the game code modifies this struct, it will result in a
 * prediction error of some degree. */
struct pmove_state_t{
	pmtype_t pm_type;
	short origin[3];	// 12.3
	short velocity[3];	// 12.3
	uchar pm_flags;	// ducked, jump_held, etc
	uchar pm_time;	// each unit = 8 ms
	short gravity;
	/* add to command angles to get view direction changed by spawns,
	 * rotating objects and teleporters */
	short delta_angles[3];
};

enum{
	BUTTON_ATTACK = 1<<0,
	BUTTON_USE = 1<<1,
	BUTTON_ANY = 1<<7,	// any key whatsoever

	MAXTOUCH = 32
};
/* sent to the server each client frame */
struct usercmd_t{
	uchar msec;
	uchar buttons;
	short angles[3];
	short forwardmove;
	short sidemove;
	short upmove;
	uchar impulse;	// remove?
	uchar lightlevel;	// light level the player is standing on
};
struct pmove_t{
	pmove_state_t s;	// state (in / out)
	/* command (in) */
	usercmd_t cmd;
	qboolean snapinitial;	// if s has been changed outside pmove
	/* results (out) */
	int numtouch;
	edict_t	*touchents[MAXTOUCH];

	vec3_t viewangles;	// clamped
	float viewheight;
	vec3_t mins;	// bounding box size
	vec3_t maxs;
	edict_t	*groundentity;
	int watertype;
	int waterlevel;
	/* callbacks to test the world */
	trace_t	(*trace)(vec3_t, vec3_t, vec3_t, vec3_t);
	int	(*pointcontents)(vec3_t);
};

enum{
	/* entity_state_t->effects
	 * Effects are things handled on the client side (lights, particles, frame
	 * animations) that happen constantly on the given entity. An entity that has
	 * effects will be sent to the client even if it has a zero index model. */
	EF_ROTATE = 1<<0,	// rotate (bonus items)
	EF_GIB = 1<<1,	// leave a trail
	EF_BLASTER = 1<<3,	// redlight + trail
	EF_ROCKET = 1<<4,	// redlight + trail
	EF_GRENADE = 1<<5,
	EF_HYPERBLASTER = 1<<6,
	EF_BFG = 1<<7,
	EF_COLOR_SHELL = 1<<8,
	EF_POWERSCREEN = 1<<9,
	EF_ANIM01 = 1<<10,	// automatically cycle between frames 0 and 1 at 2 hz
	EF_ANIM23 = 1<<11,	// automatically cycle between frames 2 and 3 at 2 hz
	EF_ANIM_ALL = 1<<12,	// automatically cycle through all frames at 2hz
	EF_ANIM_ALLFAST = 1<<13,	// automatically cycle through all frames at 10hz
	EF_FLIES = 1<<14,
	EF_QUAD = 1<<15,
	EF_PENT = 1<<16,
	EF_TELEPORTER = 1<<17,	// particle fountain
	EF_FLAG1 = 1<<18,
	EF_FLAG2 = 1<<19,
	EF_IONRIPPER = 1<<20,
	EF_GREENGIB = 1<<21,
	EF_BLUEHYPERBLASTER = 1<<22,
	EF_SPINNINGLIGHTS = 1<<23,
	EF_PLASMA = 1<<24,
	EF_TRAP = 1<<25,
	/* ROGUE */
	EF_TRACKER = 1<<26,
	EF_DOUBLE = 1<<27,
	EF_SPHERETRANS = 1<<28,
	EF_TAGTRAIL = 1<<29,
	EF_HALF_DAMAGE = 1<<30,
	EF_TRACKERTRAIL = 1<<31,

	/* entity_state_t->renderfx */
	RF_MINLIGHT = 1<<0,	// allways have some light (viewmodel)
	RF_VIEWERMODEL = 1<<1,	// don't draw through eyes, only mirrors
	RF_WEAPONMODEL = 1<<2,	// only draw through eyes
	RF_FULLBRIGHT = 1<<3,	// allways draw full intensity
	RF_DEPTHHACK = 1<<4,	// for view weapon Z crunching
	RF_TRANSLUCENT = 1<<5,
	RF_FRAMELERP = 1<<6,
	RF_BEAM = 1<<7,
	RF_CUSTOMSKIN = 1<<8,	// skin is an index in image_precache
	RF_GLOW = 1<<9,		// pulse lighting for bonus items
	RF_SHELL_RED = 1<<10,
	RF_SHELL_GREEN = 1<<11,
	RF_SHELL_BLUE = 1<<12,
	/* ROGUE */
	RF_IR_VISIBLE = 1<<15,
	RF_SHELL_DOUBLE = 1<<16,
	RF_SHELL_HALF_DAM = 1<<17,
	RF_USE_DISGUISE = 1<<18,

	/* player_state_t->refdef */
	RDF_UNDERWATER = 1<<0,	// warp the screen as apropriate
	RDF_NOWORLDMODEL = 1<<1,	// used for player configuration screen
	/* ROGUE */
	RDF_IRGOGGLES = 1<<2,
	RDF_UVGOGGLES = 1<<3,

	/* muzzle flashes, player effects */
	MZ_BLASTER = 0,
	MZ_MACHINEGUN = 1,
	MZ_SHOTGUN = 2,
	MZ_CHAINGUN1 = 3,
	MZ_CHAINGUN2 = 4,
	MZ_CHAINGUN3 = 5,
	MZ_RAILGUN = 6,
	MZ_ROCKET = 7,
	MZ_GRENADE = 8,
	MZ_LOGIN = 9,
	MZ_LOGOUT = 10,
	MZ_RESPAWN = 11,
	MZ_BFG = 12,
	MZ_SSHOTGUN = 13,
	MZ_HYPERBLASTER = 14,
	MZ_ITEMRESPAWN = 15,
	MZ_IONRIPPER = 16,
	MZ_BLUEHYPERBLASTER = 17,
	MZ_PHALANX = 18,
	MZ_SILENCED = 128,	// bit flag ORed with one of the above numbers
	/* ROGUE */
	MZ_ETF_RIFLE = 30,
	MZ_UNUSED = 31,
	MZ_SHOTGUN2 = 32,
	MZ_HEATBEAM = 33,
	MZ_BLASTER2 = 34,
	MZ_TRACKER = 35,
	MZ_NUKE1 = 36,
	MZ_NUKE2 = 37,
	MZ_NUKE4 = 38,
	MZ_NUKE8 = 39,

	/* monster muzzle flashes */
	MZ2_TANK_BLASTER_1 = 1,
	MZ2_TANK_BLASTER_2 = 2,
	MZ2_TANK_BLASTER_3 = 3,
	MZ2_TANK_MACHINEGUN_1 = 4,
	MZ2_TANK_MACHINEGUN_2 = 5,
	MZ2_TANK_MACHINEGUN_3 = 6,
	MZ2_TANK_MACHINEGUN_4 = 7,
	MZ2_TANK_MACHINEGUN_5 = 8,
	MZ2_TANK_MACHINEGUN_6 = 9,
	MZ2_TANK_MACHINEGUN_7 = 10,
	MZ2_TANK_MACHINEGUN_8 = 11,
	MZ2_TANK_MACHINEGUN_9 = 12,
	MZ2_TANK_MACHINEGUN_10 = 13,
	MZ2_TANK_MACHINEGUN_11 = 14,
	MZ2_TANK_MACHINEGUN_12 = 15,
	MZ2_TANK_MACHINEGUN_13 = 16,
	MZ2_TANK_MACHINEGUN_14 = 17,
	MZ2_TANK_MACHINEGUN_15 = 18,
	MZ2_TANK_MACHINEGUN_16 = 19,
	MZ2_TANK_MACHINEGUN_17 = 20,
	MZ2_TANK_MACHINEGUN_18 = 21,
	MZ2_TANK_MACHINEGUN_19 = 22,
	MZ2_TANK_ROCKET_1 = 23,
	MZ2_TANK_ROCKET_2 = 24,
	MZ2_TANK_ROCKET_3 = 25,
	MZ2_INFANTRY_MACHINEGUN_1 = 26,
	MZ2_INFANTRY_MACHINEGUN_2 = 27,
	MZ2_INFANTRY_MACHINEGUN_3 = 28,
	MZ2_INFANTRY_MACHINEGUN_4 = 29,
	MZ2_INFANTRY_MACHINEGUN_5 = 30,
	MZ2_INFANTRY_MACHINEGUN_6 = 31,
	MZ2_INFANTRY_MACHINEGUN_7 = 32,
	MZ2_INFANTRY_MACHINEGUN_8 = 33,
	MZ2_INFANTRY_MACHINEGUN_9 = 34,
	MZ2_INFANTRY_MACHINEGUN_10 = 35,
	MZ2_INFANTRY_MACHINEGUN_11 = 36,
	MZ2_INFANTRY_MACHINEGUN_12 = 37,
	MZ2_INFANTRY_MACHINEGUN_13 = 38,
	MZ2_SOLDIER_BLASTER_1 = 39,
	MZ2_SOLDIER_BLASTER_2 = 40,
	MZ2_SOLDIER_SHOTGUN_1 = 41,
	MZ2_SOLDIER_SHOTGUN_2 = 42,
	MZ2_SOLDIER_MACHINEGUN_1 = 43,
	MZ2_SOLDIER_MACHINEGUN_2 = 44,
	MZ2_GUNNER_MACHINEGUN_1 = 45,
	MZ2_GUNNER_MACHINEGUN_2 = 46,
	MZ2_GUNNER_MACHINEGUN_3 = 47,
	MZ2_GUNNER_MACHINEGUN_4 = 48,
	MZ2_GUNNER_MACHINEGUN_5 = 49,
	MZ2_GUNNER_MACHINEGUN_6 = 50,
	MZ2_GUNNER_MACHINEGUN_7 = 51,
	MZ2_GUNNER_MACHINEGUN_8 = 52,
	MZ2_GUNNER_GRENADE_1 = 53,
	MZ2_GUNNER_GRENADE_2 = 54,
	MZ2_GUNNER_GRENADE_3 = 55,
	MZ2_GUNNER_GRENADE_4 = 56,
	MZ2_CHICK_ROCKET_1 = 57,
	MZ2_FLYER_BLASTER_1 = 58,
	MZ2_FLYER_BLASTER_2 = 59,
	MZ2_MEDIC_BLASTER_1 = 60,
	MZ2_GLADIATOR_RAILGUN_1 = 61,
	MZ2_HOVER_BLASTER_1 = 62,
	MZ2_ACTOR_MACHINEGUN_1 = 63,
	MZ2_SUPERTANK_MACHINEGUN_1 = 64,
	MZ2_SUPERTANK_MACHINEGUN_2 = 65,
	MZ2_SUPERTANK_MACHINEGUN_3 = 66,
	MZ2_SUPERTANK_MACHINEGUN_4 = 67,
	MZ2_SUPERTANK_MACHINEGUN_5 = 68,
	MZ2_SUPERTANK_MACHINEGUN_6 = 69,
	MZ2_SUPERTANK_ROCKET_1 = 70,
	MZ2_SUPERTANK_ROCKET_2 = 71,
	MZ2_SUPERTANK_ROCKET_3 = 72,
	MZ2_BOSS2_MACHINEGUN_L1 = 73,
	MZ2_BOSS2_MACHINEGUN_L2 = 74,
	MZ2_BOSS2_MACHINEGUN_L3 = 75,
	MZ2_BOSS2_MACHINEGUN_L4 = 76,
	MZ2_BOSS2_MACHINEGUN_L5 = 77,
	MZ2_BOSS2_ROCKET_1 = 78,
	MZ2_BOSS2_ROCKET_2 = 79,
	MZ2_BOSS2_ROCKET_3 = 80,
	MZ2_BOSS2_ROCKET_4 = 81,
	MZ2_FLOAT_BLASTER_1 = 82,
	MZ2_SOLDIER_BLASTER_3 = 83,
	MZ2_SOLDIER_SHOTGUN_3 = 84,
	MZ2_SOLDIER_MACHINEGUN_3 = 85,
	MZ2_SOLDIER_BLASTER_4 = 86,
	MZ2_SOLDIER_SHOTGUN_4 = 87,
	MZ2_SOLDIER_MACHINEGUN_4 = 88,
	MZ2_SOLDIER_BLASTER_5 = 89,
	MZ2_SOLDIER_SHOTGUN_5 = 90,
	MZ2_SOLDIER_MACHINEGUN_5 = 91,
	MZ2_SOLDIER_BLASTER_6 = 92,
	MZ2_SOLDIER_SHOTGUN_6 = 93,
	MZ2_SOLDIER_MACHINEGUN_6 = 94,
	MZ2_SOLDIER_BLASTER_7 = 95,
	MZ2_SOLDIER_SHOTGUN_7 = 96,
	MZ2_SOLDIER_MACHINEGUN_7 = 97,
	MZ2_SOLDIER_BLASTER_8 = 98,
	MZ2_SOLDIER_SHOTGUN_8 = 99,
	MZ2_SOLDIER_MACHINEGUN_8 = 100,
	/* --- Xian shit below --- */
	MZ2_MAKRON_BFG = 101,
	MZ2_MAKRON_BLASTER_1 = 102,
	MZ2_MAKRON_BLASTER_2 = 103,
	MZ2_MAKRON_BLASTER_3 = 104,
	MZ2_MAKRON_BLASTER_4 = 105,
	MZ2_MAKRON_BLASTER_5 = 106,
	MZ2_MAKRON_BLASTER_6 = 107,
	MZ2_MAKRON_BLASTER_7 = 108,
	MZ2_MAKRON_BLASTER_8 = 109,
	MZ2_MAKRON_BLASTER_9 = 110,
	MZ2_MAKRON_BLASTER_10 = 111,
	MZ2_MAKRON_BLASTER_11 = 112,
	MZ2_MAKRON_BLASTER_12 = 113,
	MZ2_MAKRON_BLASTER_13 = 114,
	MZ2_MAKRON_BLASTER_14 = 115,
	MZ2_MAKRON_BLASTER_15 = 116,
	MZ2_MAKRON_BLASTER_16 = 117,
	MZ2_MAKRON_BLASTER_17 = 118,
	MZ2_MAKRON_RAILGUN_1 = 119,
	MZ2_JORG_MACHINEGUN_L1 = 120,
	MZ2_JORG_MACHINEGUN_L2 = 121,
	MZ2_JORG_MACHINEGUN_L3 = 122,
	MZ2_JORG_MACHINEGUN_L4 = 123,
	MZ2_JORG_MACHINEGUN_L5 = 124,
	MZ2_JORG_MACHINEGUN_L6 = 125,
	MZ2_JORG_MACHINEGUN_R1 = 126,
	MZ2_JORG_MACHINEGUN_R2 = 127,
	MZ2_JORG_MACHINEGUN_R3 = 128,
	MZ2_JORG_MACHINEGUN_R4 = 129,
	MZ2_JORG_MACHINEGUN_R5 = 130,
	MZ2_JORG_MACHINEGUN_R6 = 131,
	MZ2_JORG_BFG_1 = 132,
	MZ2_BOSS2_MACHINEGUN_R1 = 133,
	MZ2_BOSS2_MACHINEGUN_R2 = 134,
	MZ2_BOSS2_MACHINEGUN_R3 = 135,
	MZ2_BOSS2_MACHINEGUN_R4 = 136,
	MZ2_BOSS2_MACHINEGUN_R5 = 137,
	/* ROGUE */
	MZ2_CARRIER_MACHINEGUN_L1 = 138,
	MZ2_CARRIER_MACHINEGUN_R1 = 139,
	MZ2_CARRIER_GRENADE = 140,
	MZ2_TURRET_MACHINEGUN = 141,
	MZ2_TURRET_ROCKET = 142,
	MZ2_TURRET_BLASTER = 143,
	MZ2_STALKER_BLASTER = 144,
	MZ2_DAEDALUS_BLASTER = 145,
	MZ2_MEDIC_BLASTER_2 = 146,
	MZ2_CARRIER_RAILGUN = 147,
	MZ2_WIDOW_DISRUPTOR = 148,
	MZ2_WIDOW_BLASTER = 149,
	MZ2_WIDOW_RAIL = 150,
	MZ2_WIDOW_PLASMABEAM = 151,	// PMM - not used
	MZ2_CARRIER_MACHINEGUN_L2 = 152,
	MZ2_CARRIER_MACHINEGUN_R2 = 153,
	MZ2_WIDOW_RAIL_LEFT = 154,
	MZ2_WIDOW_RAIL_RIGHT = 155,
	MZ2_WIDOW_BLASTER_SWEEP1 = 156,
	MZ2_WIDOW_BLASTER_SWEEP2 = 157,
	MZ2_WIDOW_BLASTER_SWEEP3 = 158,
	MZ2_WIDOW_BLASTER_SWEEP4 = 159,
	MZ2_WIDOW_BLASTER_SWEEP5 = 160,
	MZ2_WIDOW_BLASTER_SWEEP6 = 161,
	MZ2_WIDOW_BLASTER_SWEEP7 = 162,
	MZ2_WIDOW_BLASTER_SWEEP8 = 163,
	MZ2_WIDOW_BLASTER_SWEEP9 = 164,
	MZ2_WIDOW_BLASTER_100 = 165,
	MZ2_WIDOW_BLASTER_90 = 166,
	MZ2_WIDOW_BLASTER_80 = 167,
	MZ2_WIDOW_BLASTER_70 = 168,
	MZ2_WIDOW_BLASTER_60 = 169,
	MZ2_WIDOW_BLASTER_50 = 170,
	MZ2_WIDOW_BLASTER_40 = 171,
	MZ2_WIDOW_BLASTER_30 = 172,
	MZ2_WIDOW_BLASTER_20 = 173,
	MZ2_WIDOW_BLASTER_10 = 174,
	MZ2_WIDOW_BLASTER_0 = 175,
	MZ2_WIDOW_BLASTER_10L = 176,
	MZ2_WIDOW_BLASTER_20L = 177,
	MZ2_WIDOW_BLASTER_30L = 178,
	MZ2_WIDOW_BLASTER_40L = 179,
	MZ2_WIDOW_BLASTER_50L = 180,
	MZ2_WIDOW_BLASTER_60L = 181,
	MZ2_WIDOW_BLASTER_70L = 182,
	MZ2_WIDOW_RUN_1 = 183,
	MZ2_WIDOW_RUN_2 = 184,
	MZ2_WIDOW_RUN_3 = 185,
	MZ2_WIDOW_RUN_4 = 186,
	MZ2_WIDOW_RUN_5 = 187,
	MZ2_WIDOW_RUN_6 = 188,
	MZ2_WIDOW_RUN_7 = 189,
	MZ2_WIDOW_RUN_8 = 190,
	MZ2_CARRIER_ROCKET_1 = 191,
	MZ2_CARRIER_ROCKET_2 = 192,
	MZ2_CARRIER_ROCKET_3 = 193,
	MZ2_CARRIER_ROCKET_4 = 194,
	MZ2_WIDOW2_BEAMER_1 = 195,
	MZ2_WIDOW2_BEAMER_2 = 196,
	MZ2_WIDOW2_BEAMER_3 = 197,
	MZ2_WIDOW2_BEAMER_4 = 198,
	MZ2_WIDOW2_BEAMER_5 = 199,
	MZ2_WIDOW2_BEAM_SWEEP_1 = 200,
	MZ2_WIDOW2_BEAM_SWEEP_2 = 201,
	MZ2_WIDOW2_BEAM_SWEEP_3 = 202,
	MZ2_WIDOW2_BEAM_SWEEP_4 = 203,
	MZ2_WIDOW2_BEAM_SWEEP_5 = 204,
	MZ2_WIDOW2_BEAM_SWEEP_6 = 205,
	MZ2_WIDOW2_BEAM_SWEEP_7 = 206,
	MZ2_WIDOW2_BEAM_SWEEP_8 = 207,
	MZ2_WIDOW2_BEAM_SWEEP_9 = 208,
	MZ2_WIDOW2_BEAM_SWEEP_10 = 209,
	MZ2_WIDOW2_BEAM_SWEEP_11 = 210
};

extern vec3_t monster_flash_offset[];

/* Temp entity events are for things that happen at a location seperate from
 * any existing entity. Temporary entity messages are explicitly constructed
 * and broadcast. */
typedef enum temp_event_t{
	TE_GUNSHOT,
	TE_BLOOD,
	TE_BLASTER,
	TE_RAILTRAIL,
	TE_SHOTGUN,
	TE_EXPLOSION1,
	TE_EXPLOSION2,
	TE_ROCKET_EXPLOSION,
	TE_GRENADE_EXPLOSION,
	TE_SPARKS,
	TE_SPLASH,
	TE_BUBBLETRAIL,
	TE_SCREEN_SPARKS,
	TE_SHIELD_SPARKS,
	TE_BULLET_SPARKS,
	TE_LASER_SPARKS,
	TE_PARASITE_ATTACK,
	TE_ROCKET_EXPLOSION_WATER,
	TE_GRENADE_EXPLOSION_WATER,
	TE_MEDIC_CABLE_ATTACK,
	TE_BFG_EXPLOSION,
	TE_BFG_BIGEXPLOSION,
	TE_BOSSTPORT,	// used as '22' in a map, so DON'T RENUMBER!!!
	TE_BFG_LASER,
	TE_GRAPPLE_CABLE,
	TE_WELDING_SPARKS,
	TE_GREENBLOOD,
	TE_BLUEHYPERBLASTER,
	TE_PLASMA_EXPLOSION,
	TE_TUNNEL_SPARKS,
	/* ROGUE */
	TE_BLASTER2,
	TE_RAILTRAIL2,
	TE_FLAME,
	TE_LIGHTNING,
	TE_DEBUGTRAIL,
	TE_PLAIN_EXPLOSION,
	TE_FLASHLIGHT,
	TE_FORCEWALL,
	TE_HEATBEAM,
	TE_MONSTER_HEATBEAM,
	TE_STEAM,
	TE_BUBBLETRAIL2,
	TE_MOREBLOOD,
	TE_HEATBEAM_SPARKS,
	TE_HEATBEAM_STEAM,
	TE_CHAINFIST_SMOKE,
	TE_ELECTRIC_SPARKS,
	TE_TRACKER_EXPLOSION,
	TE_TELEPORT_EFFECT,
	TE_DBALL_GOAL,
	TE_WIDOWBEAMOUT,
	TE_NUKEBLAST,
	TE_WIDOWSPLASH,
	TE_EXPLOSION1_BIG,
	TE_EXPLOSION1_NP,
	TE_FLECHETTE
}temp_event_t;

enum{
	SPLASH_UNKNOWN = 0,
	SPLASH_SPARKS = 1,
	SPLASH_BLUE_WATER = 2,
	SPLASH_BROWN_WATER = 3,
	SPLASH_SLIME = 4,
	SPLASH_LAVA = 5,
	SPLASH_BLOOD = 6,

	/* sound channels
	 * channel 0 never willingly overrides
	 * other channels (1-7) allways override a playing sound on that channel */
	CHAN_AUTO = 0,
	CHAN_WEAPON = 1,
	CHAN_VOICE = 2,
	CHAN_ITEM = 3,
	CHAN_BODY = 4,
	/* modifier flags */
	CHAN_NO_PHS_ADD = 1<<3,	// send to all clients, not just ones in PHS (ATTN 0 will also do this)
	CHAN_RELIABLE = 1<<4,	// send by reliable message, not datagram

	/* sound attenuation values */
	ATTN_NONE = 0,	// full volume the entire level
	ATTN_NORM = 1,
	ATTN_IDLE = 2,
	ATTN_STATIC = 3,	// diminish very rapidly with distance

	/* player_state->stats[] */
	STAT_HEALTH_ICON = 0,
	STAT_HEALTH = 1,
	STAT_AMMO_ICON = 2,
	STAT_AMMO = 3,
	STAT_ARMOR_ICON = 4,
	STAT_ARMOR = 5,
	STAT_SELECTED_ICON = 6,
	STAT_PICKUP_ICON = 7,
	STAT_PICKUP_STRING = 8,
	STAT_TIMER_ICON = 9,
	STAT_TIMER = 10,
	STAT_HELPICON = 11,
	STAT_SELECTED_ITEM = 12,
	STAT_LAYOUTS = 13,
	STAT_FRAGS = 14,
	STAT_FLASHES = 15,	// cleared each frame, 1 = health, 2 = armor
	STAT_CHASE = 16,
	STAT_SPECTATOR = 17,
	MAX_STATS = 32,

	/* dmflags->value */
	DF_NO_HEALTH = 1<<0,
	DF_NO_ITEMS = 1<<1,
	DF_WEAPONS_STAY = 1<<2,
	DF_NO_FALLING = 1<<3,
	DF_INSTANT_ITEMS = 1<<4,
	DF_SAME_LEVEL = 1<<5,
	DF_SKINTEAMS = 1<<6,
	DF_MODELTEAMS = 1<<7,
	DF_NO_FRIENDLY_FIRE = 1<<8,
	DF_SPAWN_FARTHEST = 1<<9,
	DF_FORCE_RESPAWN = 1<<10,
	DF_NO_ARMOR = 1<<11,
	DF_ALLOW_EXIT = 1<<12,
	DF_INFINITE_AMMO = 1<<13,
	DF_QUAD_DROP = 1<<14,
	DF_FIXED_FOV = 1<<15,
	DF_QUADFIRE_DROP = 1<<16,
	/* ROGUE */
	DF_NO_MINES = 1<<17,
	DF_NO_STACK_DOUBLE = 1<<18,
	DF_NO_NUKES = 1<<19,
	DF_NO_SPHERES = 1<<20,

	ROGUE_VERSION_ID = 1278	// probably bs
};

/* communicated accross the net */

#define	ANGLE2SHORT(x)	((int)((x)*65536/360) & 65535)
#define	SHORT2ANGLE(x)	((x)*(360.0/65536))

enum{
	/* config strings are a general means of communication from the server
	 * to all connected clients. each config string can be at most
	 * MAX_QPATH characters. */
	CS_NAME = 0,
	CS_CDTRACK = 1,
	CS_SKY = 2,
	CS_SKYAXIS = 3,	// %f %f %f format
	CS_SKYROTATE = 4,
	CS_STATUSBAR = 5,	// display program string
	CS_AIRACCEL = 29,	// air acceleration control
	CS_MAXCLIENTS = 30,
	CS_MAPCHECKSUM = 31,	// for catching cheater maps
	CS_MODELS = 32,
	CS_SOUNDS = CS_MODELS+MAX_MODELS,
	CS_IMAGES = CS_SOUNDS+MAX_SOUNDS,
	CS_LIGHTS = CS_IMAGES+MAX_IMAGES,
	CS_ITEMS = CS_LIGHTS+MAX_LIGHTSTYLES,
	CS_PLAYERSKINS = CS_ITEMS+MAX_ITEMS,
	CS_GENERAL = CS_PLAYERSKINS+MAX_CLIENTS,
	MAX_CONFIGSTRINGS = CS_GENERAL+MAX_GENERAL
};

/* entity events are for effects that take place relative to an existing
 * entitiy origin. Very network efficient. All muzzle flashes really should be
 * converted to events... */
typedef enum entity_event_t{
	EV_NONE,
	EV_ITEM_RESPAWN,
	EV_FOOTSTEP,
	EV_FALLSHORT,
	EV_FALL,
	EV_FALLFAR,
	EV_PLAYER_TELEPORT,
	EV_OTHER_TELEPORT
}entity_event_t;

/* information conveyed from the server in an update message about entities
 * that the client will need to render in some way */
struct entity_state_t{
	int number;			// edict index
	vec3_t origin;
	vec3_t angles;
	vec3_t old_origin;		// for lerping
	int modelindex;
	int modelindex2;	// weapons, CTF flags, etc
	int modelindex3;
	int modelindex4;
	int frame;
	int skinnum;
	uint effects;	// PGM - we're filling it, so it needs to be unsigned
	int renderfx;
	/* for client side prediction; SV_LinkEdict() sets .solid properly:
	 * 8*(bits 0-4) is x/y radius
	 * 8*(bits 5-9) is z down distance
	 * 8(bits10-15) is z up */
	int solid;
	int sound;	// for looping sounds, to guarantee shutoff
	/* impulse events: muzzle flashes, footsteps, etc.
	 * events only go out for a single frame, they are automatically
	 * cleared each frame */
	int event;
};
/* the cl_parse_entities must be large enough to hold UPDATE_BACKUP frames of
 * entities, so that when a delta compressed message arives from the server it
 * can be un-deltad from the original */
extern entity_state_t cl_parse_entities[MAX_PARSE_ENTITIES];

/* information needed in addition to pmove_state_t to render a view. There will
 * only be 10 player_state_t sent each second, but the number of pmove_state_t
 * changes will be reletive to client frame rates */
struct player_state_t{
	pmove_state_t pmove;	// for prediction
	/* these fields do not need to be communicated bit-precise */
	vec3_t viewangles;	// for fixed views
	vec3_t viewoffset;	// add to pmovestate->origin
	/* set by weapon kicks, pain effects, etc. */
	vec3_t kick_angles;	// add to view direction to get render angles
	vec3_t gunangles;
	vec3_t gunoffset;
	int gunindex;
	int gunframe;
	float blend[4];	// rgba full screen effect
	float fov;	// horizontal field of view
	int rdflags;	// refdef flags
	short stats[MAX_STATS];	// fast status bar updates
};

enum{
	VIDREF_GL = 1,
	VIDREF_SOFT = 2
};
extern int vidref_val;


#include "game.h"	// ugh
extern game_export_t *ge;


#define	VERSION 3.19
#define	BASEDIRNAME "baseq2"

struct sizebuf_t{
	qboolean allowoverflow;	// if false, do a Com_Error
	qboolean overflowed;	// set to true if the buffer size failed
	uchar *data;
	int maxsize;
	int cursize;
	int readcount;
};
extern sizebuf_t net_message;

#define DEFAULT_SOUND_PACKET_VOLUME	1.0
#define DEFAULT_SOUND_PACKET_ATTENUATION	1.0
enum{
	PROTOCOL_VERSION = 34,
	PORT_MASTER = 27900,
	PORT_CLIENT = 27901,
	PORT_SERVER = 27910,
	UPDATE_BACKUP = 16,	// buffered copies of entity_state_t; must be power of two
	UPDATE_MASK = UPDATE_BACKUP-1,

	/* server to client: protocol bytes that can be directly added to
	 * messages; the svc_strings[] array in cl_parse.c should mirror this */
	svc_bad = 0,
	/* these ops are known to the game dll */
	svc_muzzleflash,
	svc_muzzleflash2,
	svc_temp_entity,
	svc_layout,
	svc_inventory,
	/* the rest are private to the client and server */
	svc_nop,
	svc_disconnect,
	svc_reconnect,
	svc_sound,
	svc_print,	// [byte] id [string] null terminated string
	svc_stufftext,	// [string] stuffed into client's console buffer, should be \n terminated
	svc_serverdata,	// [long] protocol ...
	svc_configstring,	// [short] [string]
	svc_spawnbaseline,		
	svc_centerprint,	// [string] to put in center of the screen
	svc_download,	// [short] size [size bytes]
	svc_playerinfo,	// variable
	svc_packetentities,	// [...]
	svc_deltapacketentities,	// [...]
	svc_frame,

	/* client to server */
	clc_bad = 0,
	clc_nop, 		
	clc_move,	// [[usercmd_t]
	clc_userinfo,	// [[userinfo string]
	clc_stringcmd,	// [string] message

	/* player_state_t communication */
	PS_M_TYPE = 1<<0,
	PS_M_ORIGIN = 1<<1,
	PS_M_VELOCITY = 1<<2,
	PS_M_TIME = 1<<3,
	PS_M_FLAGS = 1<<4,
	PS_M_GRAVITY = 1<<5,
	PS_M_DELTA_ANGLES = 1<<6,
	PS_VIEWOFFSET = 1<<7,
	PS_VIEWANGLES = 1<<8,
	PS_KICKANGLES = 1<<9,
	PS_BLEND = 1<<10,
	PS_FOV = 1<<11,
	PS_WEAPONINDEX = 1<<12,
	PS_WEAPONFRAME = 1<<13,
	PS_RDFLAGS = 1<<14,

	/* user_cmd_t communication: ms and light always sent, the others are
	 * optional */
	CM_ANGLE1 = 1<<0,
	CM_ANGLE2 = 1<<1,
	CM_ANGLE3 = 1<<2,
	CM_FORWARD = 1<<3,
	CM_SIDE = 1<<4,
	CM_UP = 1<<5,
	CM_BUTTONS = 1<<6,
	CM_IMPULSE = 1<<7,

	/* a sound without an ent or pos will be a local only sound */
	SND_VOLUME = 1<<0,	// a byte
	SND_ATTENUATION = 1<<1,	// a byte
	SND_POS = 1<<2,	// three coordinates
	SND_ENT = 1<<3,	// a short 0-2: channel, 3-12: entity
	SND_OFFSET = 1<<4,	// a byte, msec offset from frame start

	/* entity_state_t communication */
	/* try to pack the common update flags into the first byte */
	U_ORIGIN1 = 1<<0,
	U_ORIGIN2 = 1<<1,
	U_ANGLE2 = 1<<2,
	U_ANGLE3 = 1<<3,
	U_FRAME8 = 1<<4,	// frame is a byte
	U_EVENT = 1<<5,
	U_REMOVE = 1<<6,	// REMOVE this entity, don't add it
	U_MOREBITS1 = 1<<7,	// read one additional byte
	U_NUMBER16 = 1<<8,	// NUMBER8 is implicit if not set
	U_ORIGIN3 = 1<<9,
	U_ANGLE1 = 1<<10,
	U_MODEL = 1<<11,
	U_RENDERFX8 = 1<<12,	// fullbright, etc
	U_EFFECTS8 = 1<<14,	// autorotate, trails, etc
	U_MOREBITS2 = 1<<15,	// read one additional byte
	U_SKIN8 = 1<<16,
	U_FRAME16 = 1<<17,	// frame is a short
	U_RENDERFX16 = 1<<18,	// 8 + 16 = 32
	U_EFFECTS16 = 1<<19,	// 8 + 16 = 32
	U_MODEL2 = 1<<20,	// weapons, flags, etc
	U_MODEL3 = 1<<21,
	U_MODEL4 = 1<<22,
	U_MOREBITS3 = 1<<23,	// read one additional byte
	U_OLDORIGIN = 1<<24,	// FIXME: get rid of this
	U_SKIN16 = 1<<25,
	U_SOUND = 1<<26,
	U_SOLID = 1<<27,
};
extern char *svc_strings[256];

typedef enum netsrc_t{
	NS_CLIENT,
	NS_SERVER
}netsrc_t;
typedef enum netadrtype_t{
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
	NA_IPX,
	NA_BROADCAST_IPX
}netadrtype_t;

#define	OLD_AVG	0.99	// total = oldtotal*OLD_AVG + new*(1-OLD_AVG)
enum{
	PORT_ANY = -1,
	MAX_MSGLEN = 1400,	// max length of a message
	PACKET_HEADER = 10,	// two ints and a short
	MAX_LATENT = 32,
	MAX_MASTERS = 8	// max recipients for heartbeat packets
};
struct netadr_t{
	netadrtype_t type;
	uchar ip[4];
	uchar ipx[10];
	ushort port;
};
extern netadr_t net_from;
extern netadr_t master_adr[MAX_MASTERS];	// address of the master server

struct netchan_t{
	qboolean fatal_error;
	netsrc_t sock;
	int dropped;	// between last packet and previous
	int last_received;	// for timeouts
	int last_sent;	// for retransmits
	netadr_t remote_address;
	int qport;	// qport value to write when transmitting
	/* sequencing variables */
	int incoming_sequence;
	int incoming_acknowledged;
	int incoming_reliable_acknowledged;	// single bit
	int incoming_reliable_sequence;	// single bit, maintained local
	int outgoing_sequence;
	int reliable_sequence;	// single bit
	int last_reliable_sequence;	// sequence number of last send
	/* reliable staging and holding areas */
	sizebuf_t message;	// writing buffer to send to server
	uchar message_buf[MAX_MSGLEN-16];	// leave space for header
	/* message is copied to this buffer when it is first transfered */
	int reliable_length;
	uchar reliable_buf[MAX_MSGLEN-16];	// unacked reliable message
};
extern uchar net_message_buffer[MAX_MSGLEN];

extern float pm_airaccelerate;

extern cvar_t *developer;
extern cvar_t *dedicated;
extern cvar_t *host_speeds;
extern cvar_t *log_stats;

enum{
	ERR_FATAL = 0,	// exit the entire game with a popup window
	ERR_DROP = 1,	// print to console and disconnect from game
	ERR_QUIT = 2,	// not an error, just a normal exit
	ERR_DISCONNECT = 2,	// don't kill server
	EXEC_NOW = 0,	// don't return until completed
	EXEC_INSERT = 1,	// insert at current position, but don't run yet
	EXEC_APPEND = 2,	// add to end of the command buffer
	PRINT_ALL = 0,
	PRINT_DEVELOPER = 1,	// only print when "developer 1"

	NUMVERTEXNORMALS = 162,

	/* thread groups */
	THin = 1,
	THsnd = 2,
	THnet = 3
};

extern FILE *log_stats_file;
/* host_speeds times */
extern int time_before_game;
extern int time_after_game;
extern int time_before_ref;
extern int time_after_ref;

extern vec3_t bytedirs[NUMVERTEXNORMALS];

extern uint sys_frame_time;

#pragma pack on

/* .pak files are just a linear collapse of a directory tree */
enum{
	IDPAKHEADER = ('K'<<24)+('C'<<16)+('A'<<8)+'P',
	MAX_FILES_IN_PACK = 4096
};
struct dpackfile_t{
	char name[56];
	int filepos;
	int filelen;
};
struct dpackheader_t{
	int ident;	// == IDPAKHEADER
	int dirofs;
	int dirlen;
};

/* PCX images */
struct pcx_t{
	char manufacturer;
	char version;
	char encoding;
	char bits_per_pixel;
	ushort xmin;
	ushort ymin;
	ushort xmax;
	ushort ymax;
	ushort hres;
	ushort vres;
	uchar palette[48];
	char reserved;
	char color_planes;
	ushort bytes_per_line;
	ushort palette_type;
	char filler[58];
	uchar data;	// unbounded
};

/* .md2 triangle model file format */
enum{
	IDALIASHEADER = ('2'<<24)+('P'<<16)+('D'<<8)+'I',
	ALIAS_VERSION = 8,
	MAX_TRIANGLES = 4096,
	MAX_VERTS = 2048,
	MAX_FRAMES = 512,
	MAX_MD2SKINS = 32,
	MAX_SKINNAME = 64
};
struct dstvert_t{
	short s;
	short t;
};
struct dtriangle_t{
	short index_xyz[3];
	short index_st[3];
};
struct dtrivertx_t{
	uchar v[3];	// scaled byte to fit in frame mins/maxs
	uchar lightnormalindex;
};
struct daliasframe_t{
	float scale[3];	// multiply byte verts by this
	float translate[3];	// then add this
	char name[16];	// frame name from grabbing
	dtrivertx_t verts[1];	// variable sized
};

/* glcmd format
 * a positive integer starts a tristrip command, followed by that many vertex
 * structures. a negative integer starts a trifan command, followed by -x
 * vertexes a zero indicates the end of the command list. a vertex consists of
 * a floating point s, a floating point t, and an integer vertex index. */
struct dmdl_t{
	int ident;
	int version;
	int skinwidth;
	int skinheight;
	int framesize;	// byte size of each frame
	int num_skins;
	int num_xyz;
	int num_st;	// greater than num_xyz for seams
	int num_tris;
	int num_glcmds;	// dwords in strip/fan command lis
	int num_frames;
	int ofs_skins;	// each skin is a MAX_SKINNAME string
	int ofs_st;	// byte offset from start for stverts
	int ofs_tris;	// offset for dtriangles
	int ofs_frames;	// offset for first frame
	int ofs_glcmds;
	int ofs_end;	// end of file
};

/* .sp2 sprite file format */
enum{
	IDSPRITEHEADER = ('2'<<24)+('S'<<16)+('D'<<8)+'I',
	SPRITE_VERSION = 2
};
struct dsprframe_t{
	int width;
	int height;
	int origin_x;
	int origin_y;	// raster coordinates inside pic
	char name[MAX_SKINNAME];	// name of pcx file
};
struct dsprite_t{
	int ident;
	int version;
	int numframes;
	dsprframe_t frames[1]; // variable sized
};

/* .wal texture file format */
enum{
	MIPLEVELS = 4
};
struct miptex_t{
	char name[32];
	uint width;
	uint height;
	uint offsets[MIPLEVELS];	// four mip maps stored
	char animname[32];	// next frame in animation chain
	int flags;
	int contents;
	int value;
};

/* .bsp file format */
enum{
	IDBSPHEADER = ('P'<<24)+('S'<<16)+('B'<<8)+'I',
	BSPVERSION = 38,

	/* upper design bounds; leaffaces, leafbrushes, planes, and verts are
	 * still bounded by 16 bit short limits */
	MAX_MAP_MODELS = 1024,
	MAX_MAP_BRUSHES = 8192,
	MAX_MAP_ENTITIES = 2048,
	MAX_MAP_ENTSTRING = 0x40000,
	MAX_MAP_TEXINFO = 8192,
	MAX_MAP_AREAS = 256,
	MAX_MAP_AREAPORTALS = 1024,
	MAX_MAP_PLANES = 65536,
	MAX_MAP_NODES = 65536,
	MAX_MAP_BRUSHSIDES = 65536,
	MAX_MAP_LEAFS = 65536,
	MAX_MAP_VERTS = 65536,
	MAX_MAP_FACES = 65536,
	MAX_MAP_LEAFFACES = 65536,
	MAX_MAP_LEAFBRUSHES = 65536,
	MAX_MAP_PORTALS = 65536,
	MAX_MAP_EDGES = 128000,
	MAX_MAP_SURFEDGES = 256000,
	MAX_MAP_LIGHTING = 0x200000,
	MAX_MAP_VISIBILITY = 0x100000,

	/* key / value pair sizes */
	MAX_KEY = 32,
	MAX_VALUE = 1024,

	LUMP_ENTITIES = 0,
	LUMP_PLANES = 1,
	LUMP_VERTEXES = 2,
	LUMP_VISIBILITY = 3,
	LUMP_NODES = 4,
	LUMP_TEXINFO = 5,
	LUMP_FACES = 6,
	LUMP_LIGHTING = 7,
	LUMP_LEAFS = 8,
	LUMP_LEAFFACES = 9,
	LUMP_LEAFBRUSHES = 10,
	LUMP_EDGES = 11,
	LUMP_SURFEDGES = 12,
	LUMP_MODELS = 13,
	LUMP_BRUSHES = 14,
	LUMP_BRUSHSIDES = 15,
	LUMP_POP = 16,
	LUMP_AREAS = 17,
	LUMP_AREAPORTALS = 18,
	HEADER_LUMPS = 19,

	/* 0-2 are axial planes */
	PLANE_X = 0,
	PLANE_Y = 1,
	PLANE_Z = 2,
	/* 3-5 are non-axial planes snapped to the nearest */
	PLANE_ANYX = 3,
	PLANE_ANYY = 4,
	PLANE_ANYZ = 5
	/* planes (x&~1) and (x&~1)+1 are always opposites */
};
struct lump_t{
	int fileofs;
	int filelen;
};
struct dheader_t{
	int ident;
	int version;	
	lump_t lumps[HEADER_LUMPS];
};

struct dmodel_t{
	float mins[3];
	float maxs[3];
	float origin[3];	// for sounds or lights
	int headnode;
	int firstface;
	/* submodels just draw faces without walking the bsp tree */
	int numfaces;
};
struct dvertex_t{
	float point[3];
};
struct dplane_t{
	float normal[3];
	float dist;
	int type;	// PLANE_X - PLANE_ANYZ ?remove? trivial to regenerate
};
struct dnode_t{
	int planenum;
	int children[2];	// negative numbers are -(leafs+1), not nodes
	short mins[3];	// for frustom culling
	short maxs[3];
	ushort firstface;
	ushort numfaces;	// counting both sides
};
struct texinfo_t{
	float vecs[2][4];	// [s/t][xyz offset]
	int flags;	// miptex flags + overrides
	int value;	// light emission, etc
	char texture[32];	// texture name (textures/*.wal)
	int nexttexinfo;	// for animations, -1 = end of chain
};

enum{
	MAXLIGHTMAPS = 4,
	ANGLE_UP = -1,
	ANGLE_DOWN = -2
};
/* note that edge 0 is never used, because negative edge nums are used for
 * counterclockwise use of the edge in a face */
struct dedge_t{
	ushort v[2];	// vertex numbers
};
struct dface_t{
	ushort planenum;
	short side;
	int firstedge;	// we must support > 64k edges
	short numedges;	
	short texinfo;
	/* lighting info */
	uchar styles[MAXLIGHTMAPS];
	int lightofs;	// start of [numstyles*surfsize] samples
};
struct dleaf_t{
	int contents;	// OR of all brushes (not needed?)
	short cluster;
	short area;
	short mins[3];	// for frustum culling
	short maxs[3];
	ushort firstleafface;
	ushort numleaffaces;
	ushort firstleafbrush;
	ushort numleafbrushes;
};
struct dbrushside_t{
	ushort planenum;	// facing out of the leaf
	short texinfo;
};
struct dbrush_t{
	int firstside;
	int numsides;
	int contents;
};

/* the visibility lump consists of a header with a count, then byte offsets for
 * the PVS and PHS of each cluster, then the raw compressed bit vectors */
enum{
	DVIS_PVS = 0,
	DVIS_PHS = 1
};
struct dvis_t{
	int numclusters;
	int bitofs[8][2];	// bitofs[numclusters][2]
};
/* each area has a list of portals that lead into other areas; when portals are
 * closed, other areas may not be visible or hearable even if the vis info says
 * that it should be */
struct dareaportal_t{
	int portalnum;
	int otherarea;
};
struct darea_t{
	int numareaportals;
	int firstareaportal;
};

#pragma pack off

struct vrect_t{
	int x;
	int y;
	int width;
	int height;
	vrect_t	*pnext;
};
struct viddef_t{
	int width;          
	int height;
	pixel_t *buffer;	// invisible buffer
	pixel_t *colormap;	// 256 * VID_GRADES size
	pixel_t *alphamap;	// 256 * 256 translucency map
	int rowbytes;	// may be > width if displayed in a window or <0 for stupid dibs
};
extern viddef_t vid;

#define REF_VERSION "SOFT 0.01"
/* skins are outline flood filled and mip mapped; pics and sprites with alpha
 * will be outline flood filled pic won't be mip mapped */

extern cvar_t *sw_aliasstats;
extern cvar_t *sw_clearcolor;
extern cvar_t *sw_drawflat;
extern cvar_t *sw_draworder;
extern cvar_t *sw_maxedges;
extern cvar_t *sw_maxsurfs;
extern cvar_t *sw_mipcap;
extern cvar_t *sw_mipscale;
extern cvar_t *sw_mode;
extern cvar_t *sw_reportsurfout;
extern cvar_t *sw_reportedgeout;
extern cvar_t *sw_stipplealpha;
extern cvar_t *sw_surfcacheoverride;
extern cvar_t *sw_waterwarp;
extern cvar_t *r_fullbright;
extern cvar_t *r_lefthand;
extern cvar_t *r_drawentities;
extern cvar_t *r_drawworld;
extern cvar_t *r_dspeeds;
extern cvar_t *r_lerpmodels;
extern cvar_t *r_speeds;
extern cvar_t *r_lightlevel;  //FIXME HACK

typedef enum imagetype_t{
	it_skin,
	it_sprite,
	it_wall,
	it_pic,
	it_sky
}imagetype_t;
struct image_t{
	char name[MAX_QPATH];	// game path, including extension
	imagetype_t type;
	int width;
	int height;
	qboolean transparent;	// true if any 255 pixels in image
	int registration_sequence;	// 0 = free
	uchar *pixels[4];	// mip levels
};

typedef enum rserr_t{
	rserr_ok,
	rserr_invalid_fullscreen,
	rserr_invalid_mode,
	rserr_unknown
}rserr_t;

/* !!! if this is changed, it must be changed in asm_draw.h too !!! */
struct oldrefdef_t{
	vrect_t vrect;	// subwindow in video for refresh
	// FIXME: not need vrect next field here?
	vrect_t aliasvrect;	// scaled Alias version
	int vrectright;
	int vrectbottom;	// right & bottom screen coords
	int aliasvrectright;
	int aliasvrectbottom;	// scaled Alias versions
	/* rightmost right edge we care about, for use in edge list */
	float vrectrightedge;
	float fvrectx;
	float fvrecty;	// for floating-point compares
	float fvrectx_adj;
	float fvrecty_adj;	// left and top edges, for clamping
	int vrect_x_adj_shift20;	// vrect.x+0.5-epsilon << 20
	int vrectright_adj_shift20;	// vrectright+0.5-epsilon << 20
	float fvrectright_adj;
	float fvrectbottom_adj;
	/* right and bottom edges, for clamping */
	float fvrectright;	// rightmost edge, for Alias clamping
	float fvrectbottom;	// bottommost edge, for Alias clamping
	/* at Z = 1.0, this many X is visible; 2.0 = 90 degrees */
	float horizontalFieldOfView;
	float xOrigin;	// should probably always be 0.5
	float yOrigin;	// between be around 0.3 to 0.5
	vec3_t vieworg;
	vec3_t viewangles;
	int ambientlight;
};
extern oldrefdef_t r_refdef;

/* d*_t structures are on-disk representations; m*_t structures are in-memory */

enum{
	SIDE_FRONT = 0,
	SIDE_BACK = 1,
	SIDE_ON = 2,

	// FIXME: differentiate from texinfo SURF_ flags
	SURF_PLANEBACK = 1<<1,
	SURF_DRAWSKY = 1<<2,	// sky brush face
	SURF_DRAWTURB = 1<<4,
	SURF_DRAWBACKGROUND = 1<<6,
	SURF_DRAWSKYBOX = 1<<7,	// sky box
	SURF_FLOW = 1<<8,

	CONTENTS_NODE = -1
};
/* !!! if this is changed, it must be changed in asm_draw.h too !!! */
struct mvertex_t{
	vec3_t position;
};
/* !!! if this is changed, it must be changed in asm_i386.h too !!! */
struct mplane_t{
	vec3_t normal;
	float dist;
	uchar type;	// for texture axis selection and fast side tests
	uchar signbits;	// signx + signy<<1 + signz<<1
	uchar pad[2];
};
/* !!! if this is changed, it must be changed in asm_draw.h too !!! */
struct medge_t{
	ushort v[2];
	uint cachededgeoffset;
};
struct mtexinfo_t{
	float vecs[2][4];
	float mipadjust;
	image_t *image;
	int flags;
	int numframes;
	mtexinfo_t *next;	// animation chain
};
struct msurface_t{
	int visframe;	// should be drawn when node is crossed
	int dlightframe;
	int dlightbits;
	mplane_t *plane;
	int flags;
	int firstedge;	// look up in model->surfedges[], negative numbers
	int numedges;	// are backwards edges
	/* surface generation data */
	surfcache_t *cachespots[MIPLEVELS];
	short texturemins[2];
	short extents[2];
	mtexinfo_t *texinfo;
	/* lighting info */
	uchar styles[MAXLIGHTMAPS];
	uchar *samples;	// [numstyles*surfsize]
	msurface_t *nextalphasurface;
};

struct mnode_t{
	/* common with leaf */
	int contents;	// CONTENTS_NODE, to differentiate from leafs
	int visframe;	// node needs to be traversed if current
	short minmaxs[6];	// for bounding box culling
	mnode_t *parent;
	/* node specific */
	mplane_t *plane;
	mnode_t *children[2];
	ushort firstsurface;
	ushort numsurfaces;
};
struct mleaf_t{
	/* common with node */
	int contents;	// wil be something other than CONTENTS_NODE
	int visframe;	// node needs to be traversed if current
	short minmaxs[6];	// for bounding box culling
	mnode_t	*parent;
	/* leaf specific */
	int cluster;
	int area;
	msurface_t **firstmarksurface;
	int nummarksurfaces;
	int key;	// BSP sequence number for leaf's contents
};

typedef enum modtype_t{
	mod_bad,
	mod_brush,
	mod_sprite,
	mod_alias
}modtype_t;
struct model_t{
	char name[MAX_QPATH];
	int registration_sequence;
	modtype_t type;
	int numframes;
	int flags;
	/* volume occupied by the model graphics */		
	vec3_t mins;
	vec3_t maxs;
	/* solid volume for clipping (sent from server) */
	qboolean clipbox;
	vec3_t clipmins;
	vec3_t clipmaxs;
	/* brush model */
	int firstmodelsurface;
	int nummodelsurfaces;
	int numsubmodels;
	dmodel_t *submodels;
	int numplanes;
	mplane_t *planes;
	int numleafs;	// number of visible leafs, not counting 0
	mleaf_t *leafs;
	int numvertexes;
	mvertex_t *vertexes;
	int numedges;
	medge_t *edges;
	int numnodes;
	int firstnode;
	mnode_t *nodes;
	int numtexinfo;
	mtexinfo_t *texinfo;
	int numsurfaces;
	msurface_t *surfaces;
	int numsurfedges;
	int *surfedges;
	int nummarksurfaces;
	msurface_t **marksurfaces;
	dvis_t *vis;
	uchar *lightdata;
	/* for alias models and sprites */
	image_t *skins[MAX_MD2SKINS];
	void *extradata;
	int extradatasize;
};

extern int registration_sequence;

#define PARTICLE_Z_CLIP	8.0
#define XCENTERING	(1.0 / 2.0)
#define YCENTERING	(1.0 / 2.0)
#define CLIP_EPSILON	0.001
#define BACKFACE_EPSILON	0.01
/* !!! if this is changed, it must be changed in asm_draw.h too !!! */
#define NEAR_CLIP	0.01
enum{
	CACHE_SIZE = 32,
	VID_CBITS = 6,
	VID_GRADES = 1<<VID_CBITS,
	MAXVERTS = 64,	// max points in a surface polygon
	/* max points in an intermediate polygon (while processing) */
	MAXWORKINGVERTS = MAXVERTS+4,
	/* !!! if this is changed, it must be changed in d_ifacea.h too !!! */
	MAXHEIGHT = 4096,
	MAXWIDTH = 4096,
	/* distance that's always guaranteed to be farther away than anything
	 * in the scene */
	INFINITE_DISTANCE = 0x10000,
	WARP_WIDTH = 320,
	WARP_HEIGHT = 240,
	MAX_LBM_HEIGHT = 480,

	/* !!! must be kept the same as in quakeasm.h !!! */
	TRANSPARENT_COLOR = 0xff,
	/* !!! if this is changed, it must be changed in d_ifacea.h too !!! */
	TURB_TEX_SIZE = 64,	// base turbulent texture size
	/* !!! if this is changed, it must be changed in d_ifacea.h too !!! */
	CYCLE = 128,	// turbulent cycle size

	SCANBUFFERPAD = 0x1000,
	DS_SPAN_LIST_END = -128,
	NUMSTACKEDGES = 2000,
	MINEDGES = NUMSTACKEDGES,
	NUMSTACKSURFACES = 1000,
	MINSURFACES = NUMSTACKSURFACES,
	MAXSPANS = 3000,

	/* finalvert_t.flags */
	ALIAS_LEFT_CLIP = 1<<0,
	ALIAS_TOP_CLIP = 1<<1,
	ALIAS_RIGHT_CLIP = 1<<2,
	ALIAS_BOTTOM_CLIP = 1<<3,
	ALIAS_Z_CLIP = 1<<4,
	ALIAS_XY_CLIP_MASK = 0xf,

	SURFCACHE_SIZE_AT_320X240 = 1024*768,
	/* value returned by R_BmodelCheckBBox() if bbox is trivially rejected */
	BMODEL_FULLY_CLIPPED = 1<<4,
	MAXALIASVERTS = 2000,    // TODO: tune this
	ALIAS_Z_CLIP_PLANE = 4,
	/* turbulence stuff */
	AMP = 0x80000,
	AMP2 = 3,
	SPEED = 20
};
struct emitpoint_t{
	float u;
	float v;
	float s;
	float t;
	float zi;
};

/* asm: if you modidify finalvert_t's definition, make sure to change the
 * associated offsets too! */
#ifdef SMALL_FINALVERT
struct finalvert_t{
	short u;
	short v;
	short s;
	short t;
	int l;
	int zi;
	int flags;
	float xyz[3];	// eye space
};
#else	// !SMALL_FINALVERT
struct finalvert_t{
	int u;
	int v;
	int s;
	int t;
	int l;
	int zi;
	int flags;
	float xyz[3];	// eye space
};
#endif	// SMALL_FINALVERT

struct affinetridesc_t{
	void *pskin;
	int pskindesc;
	int skinwidth;
	int skinheight;
	dtriangle_t *ptriangles;
	finalvert_t *pfinalverts;
	int numtriangles;
	int drawtype;
	int seamfixupX16;
	qboolean do_vis_thresh;
	int vis_thresh;
};
extern affinetridesc_t r_affinetridesc;

struct drawsurf_t{
	uchar *surfdat;	// destination for generated surface
	int rowbytes;	// destination logical width in bytes
	msurface_t *surf;	// description for surface to generate
	/* adjust for lightmap levels for dynamic lighting */
	fixed8_t lightadj[MAXLIGHTMAPS];
	image_t *image;
	int surfmip;	// mipmapped ratio of surface texels / world pixels
	int surfwidth;	// in mipmapped texels
	int surfheight;	// in mipmapped texels
};
extern drawsurf_t r_drawsurf;

struct alight_t{
	int ambientlight;
	int shadelight;
	float *plightvec;
};

/* clipped bmodel edges */
struct bedge_t{
	mvertex_t *v[2];
	bedge_t *pnext;
};

/* !!! if this is changed, it must be changed in asm_draw.h too !!! */
struct clipplane_t{
	vec3_t normal;
	float dist;
	clipplane_t *next;
	uchar leftedge;
	uchar rightedge;
	uchar reserved[2];
};

struct surfcache_t{
	surfcache_t *next;
	surfcache_t **owner;	// nil is an empty chunk of memory
	int lightadj[MAXLIGHTMAPS];	// checked for strobe flush
	int dlight;
	int size;	// including header
	uint width;
	uint height;	// DEBUG only needed for debug
	float mipscale;
	image_t *image;
	uchar data[4];	// width*height elements
};
extern surfcache_t *sc_rover;
extern surfcache_t *d_initial_rover;

/* !!! if this is changed, it must be changed in asm_draw.h too !!! */
struct espan_t{
	int u;
	int v;
	int count;
	espan_t *pnext;
};

/* used by the polygon drawer (R_POLY.C) and sprite setup code (R_SPRITE.C) */
struct polydesc_t{
	int nump;
	emitpoint_t *pverts;
	uchar *pixels;	// image
	int pixel_width;	// image width
	int pixel_height;	// image height
	vec3_t vup;
	vec3_t vright;
	vec3_t vpn;	// in worldspace, for plane eq
	float dist;
	float s_offset;
	float t_offset;
	float viewer_position[3];
	void	(*drawspanlet)(void);
	int stipple_parity;
};

// FIXME: compress, make a union if that will help
// insubmodel is only 1, flags is fewer than 32, spanstate could be a byte
struct surf_t{
	surf_t *next;	// active surface stack in r_edge.c
	surf_t *prev;	// used in r_edge.c for active surf stack
	espan_t *spans;	// pointer to linked list of spans to draw
	int key;	// sorting key (BSP order)
	int last_u;	// set during tracing
	int spanstate;	// 0 = not in span; 1 = in span; -1 in inverted span (end before start)
	int flags;	// currentface flags
	msurface_t *msurf;
	entity_t *entity;
	float nearzi;	// nearest 1/z on surface, for mipmapping
	qboolean insubmodel;
	float d_ziorigin;
	float d_zistepu;
	float d_zistepv;
	int pad[2];	// to 64 bytes
};

/* !!! if this is changed, it must be changed in asm_draw.h too !!! */
struct edge_t{
	fixed16_t u;
	fixed16_t u_step;
	edge_t *prev;
	edge_t *next;
	ushort surfs[2];
	edge_t *nextremove;
	float nearzi;
	medge_t *owner;
};

struct aliastriangleparms_t{
	finalvert_t *a;
	finalvert_t *b;
	finalvert_t *c;
};
extern aliastriangleparms_t aliastriangleparms;

struct swstate_t{
	qboolean fullscreen;
	int prev_mode;	// last valid SW mode
	uchar gammatable[256];
	uchar currentpalette[1024];

};
extern swstate_t sw_state;

extern int d_spanpixcount;
extern int r_framecount;	// sequence # of current frame since quake started
/* scale-up factor for screen u and v on Alias vertices passed to driver */
extern float r_aliasuvscale;
extern qboolean r_dowarp;
extern vec3_t r_pright;
extern vec3_t r_pup;
extern vec3_t r_ppn;
extern void *acolormap;	// FIXME: should go away
extern int c_surf;
extern uchar r_warpbuffer[WARP_WIDTH*WARP_HEIGHT];
extern float scale_for_mip;
extern qboolean d_roverwrapped;

extern float d_sdivzstepu;
extern float d_tdivzstepu;
extern float d_zistepu;
extern float d_sdivzstepv;
extern float d_tdivzstepv;
extern float d_zistepv;
extern float d_sdivzorigin;
extern float d_tdivzorigin;
extern float d_ziorigin;
extern fixed16_t sadjust;
extern fixed16_t tadjust;
extern fixed16_t bbextents;
extern fixed16_t bbextentt;

extern int d_vrectx;
extern int d_vrecty;
extern int d_vrectright_particle;
extern int d_vrectbottom_particle;
extern int d_pix_min;
extern int d_pix_max;
extern int d_pix_shift;

extern pixel_t *d_viewbuffer;
extern short *d_pzbuffer;
extern uint d_zrowbytes;
extern uint d_zwidth;
extern short *zspantable[MAXHEIGHT];
extern int d_scantable[MAXHEIGHT];
extern int d_minmip;
extern float d_scalemip[3];

/* surfaces are generated in back to front order by the bsp, so if a surf
 * pointer is greater than another one, it should be drawn in front.
 * surfaces[1] is the background, and is used as the active surface stack.
 * surfaces[0] is a dummy, because index 0 is used to indicate no surface
 * attached to an edge_t */
extern int cachewidth;
extern pixel_t *cacheblock;
extern int r_screenwidth;
extern int r_drawnpolycount;
extern int sintable[MAXWIDTH+CYCLE];
extern int intsintable[nelem(sintable)];
extern int blanktable[nelem(sintable)];
extern vec3_t vup;
extern vec3_t base_vup;
extern vec3_t vpn;
extern vec3_t base_vpn;
extern vec3_t vright;
extern vec3_t base_vright;
extern surf_t *surfaces;
extern surf_t *surface_p;
extern surf_t *surf_max;

extern vec3_t sxformaxis[4];	// s axis transformed into viewspace
extern vec3_t txformaxis[4];	// t axis transformed into viewspac
extern float xcenter;
extern float ycenter;
extern float xscale;
extern float yscale;
extern float xscaleinv;
extern float yscaleinv;
extern float xscaleshrink;
extern float yscaleshrink;
extern int ubasestep;
extern int errorterm;
extern int erroradjustup;
extern int erroradjustdown;

extern clipplane_t view_clipplanes[4];
extern int *pfrustum_indexes[4];

extern mplane_t screenedge[4];
extern vec3_t r_origin;
extern entity_t	r_worldentity;
extern model_t *currentmodel;
extern entity_t *currententity;
extern vec3_t modelorg;
extern vec3_t r_entorigin;
extern float verticalFieldOfView;
extern float xOrigin;
extern float yOrigin;
extern int r_visframecount;
extern msurface_t *r_alpha_surfaces;

extern qboolean insubmodel;

extern int c_faceclip;
extern int r_polycount;
extern int r_wholepolycount;
extern int ubasestep;
extern int errorterm;
extern int erroradjustup;
extern int erroradjustdown;
extern fixed16_t sadjust;
extern fixed16_t tadjust;
extern fixed16_t bbextents;
extern fixed16_t bbextentt;
extern mvertex_t *r_ptverts;
extern mvertex_t *r_ptvertsmax;
extern float entity_rotation[3][3];
extern int r_currentkey;
extern int r_currentbkey;
extern int r_amodels_drawn;
extern edge_t *auxedges;
extern int r_numallocatededges;
extern edge_t *r_edges;
extern edge_t *edge_p;
extern edge_t *edge_max;
extern edge_t *newedges[MAXHEIGHT];
extern edge_t *removeedges[MAXHEIGHT];
extern edge_t edge_head;	// FIXME: make stack vars when debugging done
extern edge_t edge_tail;
extern edge_t edge_aftertail;
extern int r_aliasblendcolor;
extern float aliasxscale;
extern float aliasyscale;
extern float aliasxcenter;
extern float aliasycenter;
extern int r_outofsurfaces;
extern int r_outofedges;
extern mvertex_t *r_pcurrentvertbase;
extern int r_maxvalidedgeoffset;

extern float r_time1;
extern float da_time1;
extern float da_time2;
extern float dp_time1;
extern float dp_time2;
extern float db_time1;
extern float db_time2;
extern float rw_time1;
extern float rw_time2;
extern float se_time1;
extern float se_time2;
extern float de_time1;
extern float de_time2;
extern float dv_time1;
extern float dv_time2;
extern int r_frustum_indexes[4*6];
extern int r_maxsurfsseen;
extern int r_maxedgesseen;
extern int r_cnumsurfs;
extern qboolean r_surfsonstack;
extern mleaf_t *r_viewleaf;
extern int r_viewcluster;
extern int r_oldviewcluster;
extern int r_clipflags;
extern int r_dlightframecount;
extern qboolean r_fov_greater_than_90;
extern image_t *r_notexture_mip;
extern model_t *r_worldmodel;

extern refdef_t r_newrefdef;
extern surfcache_t *sc_rover;
extern surfcache_t *sc_base;
extern void *colormap;

extern unsigned d_8to24table[256];	// base
extern mtexinfo_t *sky_texinfo[6];

extern int dumpwin;

extern cvar_t *vid_fullscreen;
extern cvar_t *vid_gamma;
extern cvar_t *scr_viewsize;
extern cvar_t *crosshair;

#define POWERSUIT_SCALE 4.0F
enum{
	API_VERSION = 3,
	MAX_DLIGHTS = 32,
	MAX_ENTITIES = 128,
	MAX_PARTICLES = 4096,

	SHELL_RED_COLOR = 0xf2,
	SHELL_GREEN_COLOR = 0xd0,
	SHELL_BLUE_COLOR = 0xf3,
	SHELL_RG_COLOR = 0xdc,
	SHELL_RB_COLOR = 0x68,
	SHELL_BG_COLOR = 0x78,
	SHELL_WHITE_COLOR = 0xd7,
	/* ROGUE */
	SHELL_DOUBLE_COLOR = 0xdf,
	SHELL_HALF_DAM_COLOR = 0x90,
	SHELL_CYAN_COLOR = 0x72,

	ENTITY_FLAGS = 68
};

struct entity_t{
	model_t *model;	// opaque type outside refresh
	float angles[3];

	/* most recent data */
	float origin[3];	// also used as RF_BEAM's "from"
	int frame;	// also used as RF_BEAM's diameter
	/* previous data for lerping */
	float oldorigin[3];	// also used as RF_BEAM's "to"
	int oldframe;

	float backlerp;	// 0.0 = current, 1.0 = old
	int skinnum;	// also used as RF_BEAM's palette index
	int lightstyle;	// for flashing entities
	float alpha;	// ignore if RF_TRANSLUCENT isn't set
	image_t	*skin;	// nil for inline skin
	int flags;
};
struct dlight_t{
	vec3_t origin;
	vec3_t color;
	float intensity;
};
struct particle_t{
	vec3_t origin;
	int color;
	float alpha;
};
struct lightstyle_t{
	float rgb[3];	// 0.0 - 2.0
	float white;	// highest of rgb
};

struct refdef_t{
	int x;	// in virtual screen coordinates
	int y;
	int width;
	int height;
	float fov_x;
	float fov_y;
	float vieworg[3];
	float viewangles[3];
	float blend[4];	// rgba 0-1 full screen blend
	float time;	// time is uesed to auto animate
	int rdflags;	// RDF_UNDERWATER, etc
	uchar *areabits;	// if not nil, only areas with set bits will be drawn

	lightstyle_t *lightstyles;	// [MAX_LIGHTSTYLES]
	int num_entities;
	entity_t *entities;
	int num_dlights;
	dlight_t *dlights;
	int num_particles;
	particle_t *particles;
};

struct refexport_t{
	int api_version;
	/* All data that will be used in a level should be registered before
	 * rendering any frames to prevent disk hits, but they can still be
	 * registered at a later time if necessary.
	 * EndRegistration will free any remaining data that wasn't registered.
	 * Any model_s or skin_s pointers from before the BeginRegistration are
	 * no longer valid after EndRegistration.
	 * Skins and images need to be differentiated, because skins are flood
	 * filled to eliminate mip map edge errors, and pics have an implicit
	 * "pics/" prepended to the name. (a pic name that starts with a slash
	 * will not use the "pics/" prefix or the ".pcx" postfix) */
	qboolean	(*Init)(void);
	void	(*Shutdown)(void);
	void	(*BeginRegistration)(char *);
	model_t*	(*RegisterModel)(char *);
	image_t*	(*RegisterSkin)(char *);
	image_t*	(*RegisterPic)(char *);
	void	(*SetSky)(char *, float, vec3_t);
	void	(*EndRegistration)(void);
	void	(*RenderFrame)(refdef_t *);
	void	(*DrawGetPicSize)(int *, int *, char *);
	void	(*DrawPic)(int, int, char *);
	void	(*DrawStretchPic)(int, int, int, int, char *);
	void	(*DrawChar)(int, int, int);
	void	(*DrawTileClear)(int, int, int, int, char *);
	void	(*DrawFill)(int, int, int, int, int);
	void	(*DrawFadeScreen)(void);
	void	(*DrawStretchRaw)(int, int, int, int, int, int, uchar *);
	void	(*CinematicSetPalette)(uchar *);
	void	(*BeginFrame)(float);
	void	(*EndFrame)(void);
};
extern refexport_t re;

struct refimport_t{
	void	(*Sys_Error)(int, char *, ...);
	void	(*Cmd_AddCommand)(char *, void(*)(void));
	void	(*Cmd_RemoveCommand)(char *);
	int	(*Cmd_Argc)(void);
	char*	(*Cmd_Argv)(int);
	void	(*Cmd_ExecuteText)(int, char *);
	void	(*Con_Printf)(int, char *, ...);
	/* files will be memory mapped read only; the returned buffer may be
	 * part of a larger pak file, or a discrete file from anywhere in the
	 * quake search path
	 * a -1 return means the file does not exist
	 * nil can be passed for buf to just determine existance */
	int	(*FS_LoadFile)(char *, void **);
	void	(*FS_FreeFile)(void *);
	char*	(*FS_Gamedir)(void);
	cvar_t*	(*Cvar_Get)(char *, char *, int);
	cvar_t*	(*Cvar_Set)(char *, char *);
	void	(*Cvar_SetValue)(char *, float);
	qboolean	(*Vid_GetModeInfo)(int *, int *, int);
	void	(*Vid_MenuInit)(void);
	void	(*Vid_NewWindow)(int, int);
};
extern refimport_t ri;

extern float scr_con_current;
extern float scr_conlines;	// lines of console to display
extern int sb_lines;
extern vrect_t scr_vrect;	// position of render window
extern char crosshair_pic[MAX_QPATH];
extern int crosshair_width;
extern int crosshair_height;

extern cvar_t *s_volume;
extern cvar_t *s_loadas8bit;
extern cvar_t *s_khz;
extern cvar_t *s_mixahead;
extern int soundtime;
extern int sound_started, num_sfx;
extern sfx_t known_sfx[];

enum{
	Nsamp = 4096,
	Sampsz = 2,
	Rate = 44100,
	Nsbuf = Sampsz * Nsamp,
	MAX_CHANNELS = 32,
	MAX_RAW_SAMPLES = 8192
};

/* !!! if this is changed, the asm code must change !!! */
struct portable_samplepair_t{
	int left;
	int right;
};
extern portable_samplepair_t s_rawsamples[MAX_RAW_SAMPLES];

struct sfxcache_t{
	int length;
	int loopstart;
	int speed;	// not needed, because converted on load?
	int width;
	int stereo;
	uchar data[1];	// variable sized
};

struct sfx_t{
	char name[MAX_QPATH];
	int registration_sequence;
	sfxcache_t *cache;
	char *truename;
};

/* a playsound_t will be generated by each call to S_StartSound, when the mixer
 * reaches playsound->begin, the playsound will be assigned to a channel */
struct playsound_t{
	playsound_t *prev;
	playsound_t *next;
	sfx_t *sfx;
	float volume;
	float attenuation;
	int entnum;
	int entchannel;
	qboolean fixed_origin;	// use origin field instead of entnum's origin
	vec3_t origin;
	uint begin;	// begin on this sample
};
extern playsound_t s_pendingplays;

struct dma_t{
	int channels;
	int samples;	// mono samples in buffer
	int submission_chunk;	// don't mix less than this #
	int samplepos;	// in mono samples
	int samplebits;
	int speed;
	uchar *buffer;
};
extern dma_t dma;

/* !!! if this is changed, the asm code must change !!! */
struct channel_t{
	sfx_t *sfx;	// sfx number
	int leftvol;	// 0-255 volume
	int rightvol;	// 0-255 volume
	int end;	// end time in global paintsamples
	int pos;	// sample position in sfx
	int looping;	// where to loop, -1 = no looping OBSOLETE?
	int entnum;	// to allow overriding a specific sound
	int entchannel;
	vec3_t origin;	// only use if fixed_origin is set
	vec_t dist_mult;	// distance multiplier (attenuation/clipK)
	int master_vol;	// 0-255 master volume
	qboolean fixed_origin;	// use origin instead of fetching entnum's origin
	qboolean autosound;	// from an entity->sound, cleared each frame
};
extern channel_t channels[MAX_CHANNELS];

struct wavinfo_t{
	int rate;
	int width;
	int channels;
	int loopstart;
	int samples;
	int dataofs;	// chunk starts this many bytes from file start
};

extern int paintedtime;
extern int s_rawend;
extern vec3_t listener_origin;
extern vec3_t listener_forward;
extern vec3_t listener_right;
extern vec3_t listener_up;

extern cvar_t *in_joystick;
extern cvar_t *lookspring;
extern cvar_t *lookstrafe;
extern cvar_t *sensitivity;
extern cvar_t *freelook;
extern cvar_t *m_pitch;

/* key numbers passed to Key_Event() */
enum{
	K_TAB = 9,
	K_ENTER = 13,
	K_ESCAPE = 27,
	K_SPACE = 32,
	/* normal keys should be passed as lowercased ascii */
	K_BACKSPACE = 127,
	K_UPARROW = 128,
	K_DOWNARROW = 129,
	K_LEFTARROW = 130,
	K_RIGHTARROW = 131,
	K_ALT = 132,
	K_CTRL = 133,
	K_SHIFT = 134,
	K_F1 = 135,
	K_F2 = 136,
	K_F3 = 137,
	K_F4 = 138,
	K_F5 = 139,
	K_F6 = 140,
	K_F7 = 141,
	K_F8 = 142,
	K_F9 = 143,
	K_F10 = 144,
	K_F11 = 145,
	K_F12 = 146,
	K_INS = 147,
	K_DEL = 148,
	K_PGDN = 149,
	K_PGUP = 150,
	K_HOME = 151,
	K_END = 152,
	K_KP_HOME = 160,
	K_KP_UPARROW = 161,
	K_KP_PGUP = 162,
	K_KP_LEFTARROW = 163,
	K_KP_5 = 164,
	K_KP_RIGHTARROW = 165,
	K_KP_END = 166,
	K_KP_DOWNARROW = 167,
	K_KP_PGDN = 168,
	K_KP_ENTER = 169,
	K_KP_INS    = 170,
	K_KP_DEL = 171,
	K_KP_SLASH = 172,
	K_KP_MINUS = 173,
	K_KP_PLUS = 174,
	/* mouse buttons generate virtual keys */
	K_MOUSE1 = 200,
	K_MOUSE3 = 201,
	K_MOUSE2 = 202,
	K_MWHEELUP = 203,
	K_MWHEELDOWN = 204,
	/* joystick buttons */
	K_JOY1 = 205,
	K_JOY2 = 206,
	K_JOY3 = 207,
	K_JOY4 = 208,
	/* aux keys are for multi-buttoned joysticks to generate so they can
	 * use the normal binding process */
	K_AUX1 = 209,
	K_AUX2 = 210,
	K_AUX3 = 211,
	K_AUX4 = 212,
	K_AUX5 = 213,
	K_AUX6 = 214,
	K_AUX7 = 215,
	K_AUX8 = 216,
	K_AUX9 = 217,
	K_AUX10 = 218,
	K_AUX11 = 219,
	K_AUX12 = 220,
	K_AUX13 = 221,
	K_AUX14 = 222,
	K_AUX15 = 223,
	K_AUX16 = 224,
	K_AUX17 = 225,
	K_AUX18 = 226,
	K_AUX19 = 227,
	K_AUX20 = 228,
	K_AUX21 = 229,
	K_AUX22 = 230,
	K_AUX23 = 231,
	K_AUX24 = 232,
	K_AUX25 = 233,
	K_AUX26 = 234,
	K_AUX27 = 235,
	K_AUX28 = 236,
	K_AUX29 = 237,
	K_AUX30 = 238,
	K_AUX31 = 239,
	K_AUX32 = 240,
	K_PAUSE = 255
};
extern char *keybindings[256];
extern int anykeydown;
extern char chat_buffer[];
extern int chat_bufferlen;
extern qboolean chat_team;

enum{
	NUM_CON_TIMES = 4,
	CON_TEXTSIZE = 32768
};
struct console_t{
	qboolean initialized;
	char text[CON_TEXTSIZE];
	int current;	// line where next message will be printed
	int x;	// offset in current line for next print
	int display;	// bottom of console displays this line
	int ormask;	// high bit mask for colored characters
	int linewidth;	// characters across screen
	int totallines;	// total lines in console scrollback
	float cursorspeed;
	int vislines;
	/* cls.realtime time the line was generated for transparent notify lines */
	float times[NUM_CON_TIMES];
};
extern console_t con;

extern cvar_t *cl_stereo_separation;
extern cvar_t *cl_stereo;
extern cvar_t *cl_gun;
extern cvar_t *cl_add_blend;
extern cvar_t *cl_add_lights;
extern cvar_t *cl_add_particles;
extern cvar_t *cl_add_entities;
extern cvar_t *cl_predict;
extern cvar_t *cl_footsteps;
extern cvar_t *cl_noskins;
extern cvar_t *cl_autoskins;
extern cvar_t *cl_upspeed;
extern cvar_t *cl_forwardspeed;
extern cvar_t *cl_sidespeed;
extern cvar_t *cl_yawspeed;
extern cvar_t *cl_pitchspeed;
extern cvar_t *cl_run;
extern cvar_t *cl_anglespeedkey;
extern cvar_t *cl_shownet;
extern cvar_t *cl_showmiss;
extern cvar_t *cl_showclamp;
extern cvar_t *cl_lightlevel;	// FIXME HACK
extern cvar_t *cl_paused;
extern cvar_t *cl_timedemo;
extern cvar_t *cl_vwep;

struct frame_t{
	qboolean valid;	// cleared if delta parsing was invalid
	int serverframe;
	int servertime;	// server time the message is valid for (in msec)
	int deltaframe;
	uchar areabits[MAX_MAP_AREAS/8];	// portalarea visibility bits
	player_state_t playerstate;
	int num_entities;
	int parse_entities;	// non-masked index into cl_parse_entities array
};

struct centity_t{
	entity_state_t baseline;	// delta from this if not from a previous frame
	entity_state_t current;
	entity_state_t prev;	// will always be valid, but can be just a copy of current
	int serverframe;	// if not current, this ent isn't in the frame
	int trailcount;	// for diminishing grenade trails
	vec3_t lerp_origin;	// for trails (variable hz)
	int fly_stoptime;
};
extern centity_t cl_entities[MAX_EDICTS];

enum{
	MAX_CLIENTWEAPONMODELS = 20
};
struct clientinfo_t{
	char name[MAX_QPATH];
	char cinfo[MAX_QPATH];
	image_t	*skin;
	image_t	*icon;
	char iconname[MAX_QPATH];
	model_t	*model;
	model_t	*weaponmodel[MAX_CLIENTWEAPONMODELS];
};
extern char cl_weaponmodels[MAX_CLIENTWEAPONMODELS][MAX_QPATH];
extern int num_cl_weaponmodels;

enum{
	CMD_BACKUP = 64	// allow a lot of command backups for very fast systems
};
/* client_state_t is wiped completely at every server map change */
struct client_state_t{
	int timeoutcount;
	int timedemo_frames;
	int timedemo_start;
	qboolean refresh_prepped;	// false if on new level or new ref dll
	qboolean sound_prepped;	// ambient sounds can start
	qboolean force_refdef;	// vid has changed, so we can't use a paused refdef
	int parse_entities;	// index (not anded off) into cl_parse_entities[]
	usercmd_t cmd;
	usercmd_t cmds[CMD_BACKUP];	// each mesage will send several old cmds
	int cmd_time[CMD_BACKUP];	// time sent, for calculating pings
	short predicted_origins[CMD_BACKUP][3];	// for debug comparing against server
	float predicted_step;	// for stair up smoothing
	uint predicted_step_time;
	vec3_t predicted_origin;	// generated by CL_PredictMovement
	vec3_t predicted_angles;
	vec3_t prediction_error;
	frame_t frame;	// received from server
	int surpressCount;	// number of messages rate supressed
	frame_t frames[UPDATE_BACKUP];

	/* the client maintains its own idea of view angles, which are sent to
	 * the server each frame. it is cleared to 0 upon entering each level.
	 * the server sends a delta each frame which is added to the locally
	 * tracked view angles to account for standing on rotating objects, and
	 * teleport direction changes. */
	vec3_t viewangles;
	/* time that the client is rendering at. always <= cls.realtime */
	int time;	// this is the time value that the client
	float lerpfrac;	// between oldframe and frame
	refdef_t refdef;
	vec3_t v_forward;
	vec3_t v_right;
	vec3_t v_up;	// set when refdef.angles is set

	/* transient data from server */
	char layout[1024];	// general 2D overlay
	int inventory[MAX_ITEMS];

	// FIXME: move this cinematic stuff into the cin_t structure
	/* non-gameserver infornamtion */
	FILE *cinematic_file;
	int cinematictime;	// cls.realtime for first cinematic frame
	int cinematicframe;
	char cinematicpalette[768];
	qboolean cinematicpalette_active;

	/* server state information */
	qboolean attractloop;	// running the attract loop, any key will menu
	int servercount;	// server identification for prespawns
	char gamedir[MAX_QPATH];
	int playernum;
	char configstrings[MAX_CONFIGSTRINGS][MAX_QPATH];

	/* locally derived information from server state */
	model_t *model_draw[MAX_MODELS];
	cmodel_t *model_clip[MAX_MODELS];
	sfx_t *sound_precache[MAX_SOUNDS];
	image_t *image_precache[MAX_IMAGES];
	clientinfo_t clientinfo[MAX_CLIENTS];
	clientinfo_t baseclientinfo;
};
extern client_state_t cl;

typedef enum connstate_t{
	ca_uninitialized,
	ca_disconnected,	// not talking to a server
	ca_connecting,	// sending request packets to the server
	ca_connected,	// netchan_t established, waiting for svc_serverdata
	ca_active	// game views should be displayed
}connstate_t;
typedef enum dltype_t{
	dl_none,
	dl_model,
	dl_sound,
	dl_skin,
	dl_single
}dltype_t;
typedef enum keydest_t{
	key_game,
	key_console,
	key_message,
	key_menu
}keydest_t;
/* the client_static_t structure is persistant through an arbitrary number of
 * server connections */
struct client_static_t{
	connstate_t state;
	keydest_t key_dest;
	int framecount;
	int realtime;	// always increasing, no clamping, etc
	float frametime;	// seconds since last frame

	/* screen rendering information */
	/* showing loading plaque between levels; if time gets >30 sec ahead,
	 * break it */
	float disable_screen;
	/* on receiving a frame and cl.servercount > cls.disable_servercount,
	 * clear disable_screen */
	int disable_servercount;

	/* connection information */
	char servername[MAX_OSPATH];	// name of server from original connect
	float connect_time;	// for connection retransmits
	/* ushort allowing servers to work around address translating routers */
	int quakePort;
	netchan_t netchan;
	int serverProtocol;	// in case we are doing some kind of version hack
	int challenge;	// from the server to use for connecting

	FILE *download;			// file transfer from server
	char downloadtempname[MAX_OSPATH];
	char downloadname[MAX_OSPATH];
	int downloadnumber;
	dltype_t downloadtype;
	int downloadpercent;

	/* demo recording info must be here, so it isn't cleared on level change */
	qboolean demorecording;
	qboolean demowaiting;	// don't record until a non-delta message is received
	FILE *demofile;
};
extern client_static_t cls;

struct cdlight_t{
	int key;	// so entities can reuse same entry
	vec3_t color;
	vec3_t origin;
	float radius;
	float die;	// stop lighting after this time
	float decay;	// drop this each second
	float minlight;	// don't add when contributing less
};
extern cdlight_t cl_dlights[MAX_DLIGHTS];

enum{
	MAX_SUSTAINS = 32
};
struct cl_sustain_t{
	int id;
	int type;
	int endtime;
	int nextthink;
	int thinkinterval;
	vec3_t org;
	vec3_t dir;
	int color;
	int count;
	int magnitude;
	void	(*think)(cl_sustain_t *);
};

#define INSTANT_PARTICLE -10000.0
enum{
	PARTICLE_GRAVITY = 40,
	BLASTER_PARTICLE_COLOR = 0xe0
};
struct cparticle_t{
	cparticle_t *next;
	float time;
	vec3_t org;
	vec3_t vel;
	vec3_t accel;
	float color;
	float colorvel;
	float alpha;
	float alphavel;
};

struct kbutton_t{
	int down[2];	// key nums holding it down
	uint downtime;	// msec timestamp
	uint msec;	// msec down this frame
	int state;
};
extern kbutton_t in_mlook;
extern kbutton_t in_klook;
extern kbutton_t in_strafe;
extern kbutton_t in_speed;

extern int gun_frame;
extern model_t *gun_model;

enum{
	MAXMENUITEMS = 64,
	MTYPE_SLIDER = 0,
	MTYPE_LIST = 1,
	MTYPE_ACTION = 2,
	MTYPE_SPINCONTROL = 3,
	MTYPE_SEPARATOR   = 4,
	MTYPE_FIELD = 5,
	QMF_LEFT_JUSTIFY = 1<<0,
	QMF_GRAYED = 1<<1,
	QMF_NUMBERSONLY = 1<<2,
};
struct menuframework_t{
	int x;
	int y;
	int cursor;
	int nitems;
	int nslots;
	void *items[64];
	char *statusbar;
	void	(*cursordraw)(menuframework_t *);
};
struct menucommon_t{
	int type;
	char *name;
	int x;
	int y;
	menuframework_t *parent;
	int cursor_offset;
	int localdata[4];
	uint flags;
	char *statusbar;
	void	(*callback)(void *);
	void	(*statusbarfunc)(void *);
	void	(*ownerdraw)(void *);
	void	(*cursordraw)(void *);
};
struct menufield_t{
	menucommon_t generic;
	char buffer[80];
	int cursor;
	int length;
	int visible_length;
	int visible_offset;
};
struct menuslider_t{
	menucommon_t generic;
	float minvalue;
	float maxvalue;
	float curvalue;
	float range;
};
struct menulist_t{
	menucommon_t generic;
	int curvalue;
	char **itemnames;
};
struct menuaction_t{
	menucommon_t generic;
};
struct menuseparator_t{
	menucommon_t generic;
};

extern cvar_t *sv_paused;
extern cvar_t *maxclients;
/* don't reload level state when reentering */
extern cvar_t *sv_noreload;
/* don't reload level state when reentering development tool */
extern cvar_t *sv_airaccelerate;
extern cvar_t *sv_enforcetime;

typedef enum redirect_t{
	RD_NONE,
	RD_CLIENT,
	RD_PACKET
}redirect_t;

typedef enum server_state_t{
	ss_dead,	// no map loaded
	ss_loading,	// spawning level edicts
	ss_game,	// actively running
	ss_cinematic,
	ss_demo,
	ss_pic
}server_state_t;
/* some qc commands are only valid before the server has finished initializing
 * (precache commands, static sounds / objects, etc) */
struct server_t{
	server_state_t state;	// precache commands are only valid during load
	qboolean attractloop;	// running cinematics and demos for the local system only
	qboolean loadgame;	// client begins should reuse existing entity
	uint time;	// always sv.framenum * 100 msec
	int framenum;
	char name[MAX_QPATH];	// map name, or cinematic name
	cmodel_t *models[MAX_MODELS];
	char configstrings[MAX_CONFIGSTRINGS][MAX_QPATH];
	entity_state_t baselines[MAX_EDICTS];
	/* the multicast buffer is used to send a message to a set of clients
	 * it is only used to marshall data until SV_Multicast is called */
	sizebuf_t multicast;
	uchar multicast_buf[MAX_MSGLEN];
	/* demo server information */
	FILE *demofile;
	qboolean timedemo;	// don't time sync
};
extern server_t sv;	// local server

#define EDICT_NUM(n) ((edict_t *)((uchar *)ge->edicts + ge->edict_size*(n)))
#define NUM_FOR_EDICT(e) ( ((uchar *)(e)-(uchar *)ge->edicts ) / ge->edict_size)

struct client_frame_t{
	int areabytes;
	uchar areabits[MAX_MAP_AREAS/8];	// portalarea visibility bits
	player_state_t ps;
	int num_entities;
	int first_entity;	// into the circular sv_packet_entities[]
	int senttime;	// for ping calculations
};

typedef enum clstate_t{
	cs_free,	// can be reused for a new connection
	cs_zombie,	// client disconnected, don't reuse connection for a couple secs
	cs_connected,	// has been assigned to a client_t, but not in game yet
	cs_spawned	// client is fully in game
}clstate_t;

enum{
	LATENCY_COUNTS = 16,
	RATE_MESSAGES = 10
};
struct client_t{
	clstate_t state;
	char userinfo[MAX_INFO_STRING];	// name, etc
	int lastframe;	// for delta compression
	usercmd_t lastcmd;	// for filling in big drops
	/* every seconds this is reset, if user commands exhaust it, assume
	 * time cheating */
	int commandMsec;
	int frame_latency[LATENCY_COUNTS];
	int ping;
	int message_size[RATE_MESSAGES];	// used to rate drop packets
	int rate;
	int surpressCount;	// number of messages rate supressed
	edict_t *edict;	// EDICT_NUM(clientnum+1)
	char name[32];	// extracted from userinfo, high bits masked
	int messagelevel;	// for filtering printed messages
	/* the datagram is written to by sound calls, prints, temp ents, etc.
	 * it can be harmlessly overflowed. */
	sizebuf_t datagram;
	uchar datagram_buf[MAX_MSGLEN];
	client_frame_t frames[UPDATE_BACKUP];	// updates can be delta'd from here
	uchar *download;	// file being downloaded
	int downloadsize;	// total bytes (can't use EOF because of paks)
	int downloadcount;	// bytes sent
	int lastmessage;	// sv.framenum when packet was last received
	int lastconnect;
	int challenge;	// challenge of this user, randomly generated
	netchan_t netchan;
};
extern client_t *sv_client;

/* a client can leave the server in one of four ways: dropping properly by
 * quiting or disconnecting; timing out if no valid messages are received for
 * timeout.value seconds; getting kicked off by the server operator; a program
 * error, like an overflowed reliable buffer */
enum{
	/* for preventing denial of service attacks that could cycle all of
	 * them out before legitimate users connected */
	MAX_CHALLENGES = 1024,
	SV_OUTPUTBUF_LENGTH = MAX_MSGLEN - 16
};
struct challenge_t{
	netadr_t adr;
	int challenge;
	int time;
};
struct server_static_t{
	qboolean initialized;	// sv_init has completed
	int realtime;	// always increasing, no clamping, etc
	char mapcmd[MAX_TOKEN_CHARS];	// ie: *intro.cin+base 
	int spawncount;	// incremented each server start used to check late spawns
	client_t *clients;	// [maxclients->value];
	int num_client_entities;	// maxclients->value*UPDATE_BACKUP*MAX_PACKET_ENTITIES
	int next_client_entities;	// next client_entity to use
	entity_state_t *client_entities;	// [num_client_entities]
	int last_heartbeat;
	challenge_t challenges[MAX_CHALLENGES];	// to prevent invalid IPs from connecting
	/* serverrecord values */
	FILE *demofile;
	sizebuf_t demo_multicast;
	uchar demo_multicast_buf[MAX_MSGLEN];
};
extern server_static_t svs;	// persistant server info

extern edict_t *sv_player;
extern char sv_outputbuf[SV_OUTPUTBUF_LENGTH];
