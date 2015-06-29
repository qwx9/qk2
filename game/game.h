typedef struct link_t link_t;
typedef struct game_import_t game_import_t;
typedef struct game_export_t game_export_t;
typedef struct gitem_armor_t gitem_armor_t;
typedef struct gitem_t gitem_t;
typedef struct game_locals_t game_locals_t;
typedef struct level_locals_t level_locals_t;
typedef struct spawn_temp_t spawn_temp_t;
typedef struct moveinfo_t moveinfo_t;
typedef struct mframe_t mframe_t;
typedef struct mmove_t mmove_t;
typedef struct monsterinfo_t monsterinfo_t;
typedef struct field_t field_t;
typedef struct client_persistant_t client_persistant_t;
typedef struct client_respawn_t client_respawn_t;
typedef struct edict_t edict_t;
typedef struct gclient_t gclient_t;

extern cvar_t *maxentities;
extern cvar_t *deathmatch;
extern cvar_t *coop;
extern cvar_t *dmflags;
extern cvar_t *skill;
extern cvar_t *fraglimit;
extern cvar_t *timelimit;
extern cvar_t *password;
extern cvar_t *spectator_password;
extern cvar_t *g_select_empty;
extern cvar_t *dedicated;
extern cvar_t *filterban;
extern cvar_t *sv_gravity;
extern cvar_t *sv_maxvelocity;
extern cvar_t *gun_x, *gun_y, *gun_z;
extern cvar_t *sv_rollspeed;
extern cvar_t *sv_rollangle;
extern cvar_t *run_pitch;
extern cvar_t *run_roll;
extern cvar_t *bob_up;
extern cvar_t *bob_pitch;
extern cvar_t *bob_roll;
extern cvar_t *sv_cheats;
extern cvar_t *maxclients;
extern cvar_t *maxspectators;
extern cvar_t *flood_msgs;
extern cvar_t *flood_persecond;
extern cvar_t *flood_waitdelay;
extern cvar_t *sv_maplist;

enum{
	GAME_API_VERSION = 3,

	/* edict->svflags */
	SVF_NOCLIENT = 1<<0,	// don't send entity to clients, even if it has effects
	SVF_DEADMONSTER = 1<<1,	// treat as CONTENTS_DEADMONSTER for collision
	SVF_MONSTER = 1<<2,	// treat as CONTENTS_MONSTER for collision

	MAX_ENT_CLUSTERS = 16
};

/* edict->solid values */
typedef enum solid_t{
	SOLID_NOT,	// no interaction with other objects
	SOLID_TRIGGER,	// only touch when inside, after moving
	SOLID_BBOX,	// touch on edge
	SOLID_BSP	// bsp clip, touch on edge
}solid_t;

/* link_t is only used for entity area links now */
struct link_t{
	link_t *prev;
	link_t *next;
};

struct game_import_t{
	void	(*bprintf)(int, char *, ...);
	void	(*dprintf)(char *, ...);
	void	(*cprintf)(edict_t *, int, char *, ...);
	void	(*centerprintf)(edict_t *, char *, ...);
	void	(*sound)(edict_t *, int, int, float, float, float);
	void	(*positioned_sound)(vec3_t, edict_t *, int, int, float, float, float);
	/* config strings hold all the index strings, the lightstyles, and misc
	 * data like the sky definition and cdtrack. all of the current
	 * configstrings are sent to clients when they connect, and changes are
	 * sent to all connected clients. */
	void	(*configstring)(int, char *);
	void	(*error)(char *, ...);
	/* the *index functions create configstrings and some internal server
	 * state */
	int	(*modelindex)(char *);
	int	(*soundindex)(char *);
	int	(*imageindex)(char *);
	void	(*setmodel)(edict_t *, char *);
	/* collision detection */
	trace_t	(*trace)(vec3_t, vec3_t, vec3_t, vec3_t, edict_t *, int);
	int	(*pointcontents)(vec3_t);
	qboolean	(*inPVS)(vec3_t, vec3_t);
	qboolean	(*inPHS)(vec3_t, vec3_t);
	void	(*SetAreaPortalState)(int, qboolean);
	qboolean	(*AreasConnected)(int, int);
	/* an entity will never be sent to a client or used for collision if it
	 * is not passed to linkentity.  If the size, position, or solidity
	 * changes, it must be relinked. */
	void	(*linkentity)(edict_t *);
	/* call before removing an interactive edict */
	void	(*unlinkentity)(edict_t *);
	int	(*BoxEdicts)(vec3_t, vec3_t, edict_t **, int, int);
	/* player movement code common with client prediction */
	void	(*Pmove)(pmove_t *);
	/* network messaging */
	void	(*multicast)(vec3_t, multicast_t);
	void	(*unicast)(edict_t *, qboolean);
	void	(*WriteChar)(int);
	void	(*WriteByte)(int);
	void	(*WriteShort)(int);
	void	(*WriteLong)(int);
	void	(*WriteFloat)(float);
	void	(*WriteString)(char *);
	void	(*WritePosition)(vec3_t);	// some fractional bits
	void	(*WriteDir)(vec3_t);	// single byte encoded, very coarse
	void	(*WriteAngle)(float);
	/* managed memory allocation */
	void*	(*TagMalloc)(int, int);
	void	(*TagFree)(void *);
	void	(*FreeTags)(int);
	/* console variable interaction */
	cvar_t*	(*cvar)(char *, char *, int);
	cvar_t*	(*cvar_set)(char *, char *);
	cvar_t*	(*cvar_forceset)(char *, char *);
	/* ClientCommand and ServerCommand parameter access */
	int	(*argc)(void);
	char*	(*argv)(int);
	char*	(*args)(void);	// concatenation of all argv >= 1
	/* add commands to the server console as if they were typed in for map
	 * changing, etc */
	void	(*AddCommandString)(char *);
	void	(*DebugGraph)(float, int);
};
extern game_import_t gi;

struct game_export_t{
	int apiversion;
	/* the init function will only be called when a game starts, not each
	 * time a level is loaded.  Persistant data for clients and the server
	 * can be allocated in init */
	void	(*Init)(void);
	void	(*Shutdown)(void);
	/* each new level entered will cause a call to SpawnEntities */
	void	(*SpawnEntities)(char *, char *, char *);
	/* Read/Write Game is for storing persistant cross level information
	 * about the world state and the clients. WriteGame is called every
	 * time a level is exited. ReadGame is called on a loadgame. */
	void	(*WriteGame)(char *, qboolean);
	void	(*ReadGame)(char *);
	/* ReadLevel is called after the default map information has been
	 * loaded with SpawnEntities */
	void	(*WriteLevel)(char *);
	void	(*ReadLevel)(char *);
	qboolean	(*ClientConnect)(edict_t *, char *);
	void	(*ClientBegin)(edict_t *);
	void	(*ClientUserinfoChanged)(edict_t *, char *);
	void	(*ClientDisconnect)(edict_t *);
	void	(*ClientCommand)(edict_t *);
	void	(*ClientThink)(edict_t *, usercmd_t *);
	void	(*RunFrame)(void);
	/* ServerCommand will be called when an "sv <command>" command is
	 * issued on the server console. the game can issue gi.argc() or
	 * gi.argv() commands to get the rest of the parameters */
	void	(*ServerCommand)(void);
	/* global variables shared between game and server: the edict array is
	 * allocated in the game dll so it can vary in size from one game to
	 * another. the size will be fixed when ge->Init() is called */
	edict_t *edicts;
	int edict_size;
	int num_edicts;	// current number, <= max_edicts
	int max_edicts;
};
extern game_export_t globals;

game_export_t *GetGameAPI(game_import_t *);

#define	GAMEVERSION	"baseq2"
#define DAMAGE_TIME	0.5	// view pitching times
#define	FALL_TIME	0.3
#define	FRAMETIME	0.1
enum{
	/* edict->spawnflags */
	SPAWNFLAG_NOT_EASY = 1<<8,
	SPAWNFLAG_NOT_MEDIUM = 1<<9,
	SPAWNFLAG_NOT_HARD = 1<<10,
	SPAWNFLAG_NOT_DEATHMATCH = 1<<11,
	SPAWNFLAG_NOT_COOP = 1<<12,
	/* edict->flags */
	FL_FLY = 1<<0,
	FL_SWIM = 1<<1,	// implied immunity to drowining
	FL_IMMUNE_LASER = 1<<2,
	FL_INWATER = 1<<3,
	FL_GODMODE = 1<<4,
	FL_NOTARGET = 1<<5,
	FL_IMMUNE_SLIME = 1<<6,
	FL_IMMUNE_LAVA = 1<<7,
	FL_PARTIALGROUND = 1<<8,	// not all corners are valid
	FL_WATERJUMP = 1<<9,	// player jumping out of water
	FL_TEAMSLAVE = 1<<10,	// not the first on the team
	FL_NO_KNOCKBACK = 1<<11,
	FL_POWER_ARMOR = 1<<12,	// power armor (if any) is active
	FL_RESPAWN = 1<<31,	// used for item respawning

	/* memory tags to allow dynamic memory to be cleaned up */
	TAG_GAME = 765,	// clear when unloading the dll
	TAG_LEVEL = 766,	// clear when loading a new level
	MELEE_DISTANCE = 80,
	BODY_QUEUE_SIZE = 8,

	/* deadflag */
	DEAD_NO = 0,
	DEAD_DYING = 1,
	DEAD_DEAD = 2,
	DEAD_RESPAWNABLE = 3,
	/* range */
	RANGE_MELEE = 0,
	RANGE_NEAR = 1,
	RANGE_MID = 2,
	RANGE_FAR = 3,
	/* gib types */
	GIB_ORGANIC = 0,
	GIB_METALLIC = 1,
	/* monster ai flags */
	AI_STAND_GROUND = 1<<0,
	AI_TEMP_STAND_GROUND = 1<<1,
	AI_SOUND_TARGET = 1<<2,
	AI_LOST_SIGHT = 1<<3,
	AI_PURSUIT_LAST_SEEN = 1<<4,
	AI_PURSUE_NEXT = 1<<5,
	AI_PURSUE_TEMP = 1<<6,
	AI_HOLD_FRAME = 1<<7,
	AI_GOOD_GUY = 1<<8,
	AI_BRUTAL = 1<<9,
	AI_NOSTEP = 1<<10,
	AI_DUCKED = 1<<11,
	AI_COMBAT_POINT = 1<<12,
	AI_MEDIC = 1<<13,
	AI_RESURRECTING = 1<<14,
	/* monster attack state */
	AS_STRAIGHT = 1,
	AS_SLIDING = 2,
	AS_MELEE = 3,
	AS_MISSILE = 4,
	/* armor types */
	ARMOR_NONE = 0,
	ARMOR_JACKET = 1,
	ARMOR_COMBAT = 2,
	ARMOR_BODY = 3,
	ARMOR_SHARD = 4,
	/* power armor types */
	POWER_ARMOR_NONE = 0,
	POWER_ARMOR_SCREEN = 1,
	POWER_ARMOR_SHIELD = 2,
	/* handedness values */
	RIGHT_HANDED = 0,
	LEFT_HANDED = 1,
	CENTER_HANDED = 2,
	/* game.serverflags values */
	SFL_CROSS_TRIGGER_1 = 1<<0,
	SFL_CROSS_TRIGGER_2 = 1<<1,
	SFL_CROSS_TRIGGER_3 = 1<<2,
	SFL_CROSS_TRIGGER_4 = 1<<3,
	SFL_CROSS_TRIGGER_5 = 1<<4,
	SFL_CROSS_TRIGGER_6 = 1<<5,
	SFL_CROSS_TRIGGER_7 = 1<<6,
	SFL_CROSS_TRIGGER_8 = 1<<7,
	SFL_CROSS_TRIGGER_MASK = 0xff,
	/* noise types for PlayerNoise */
	PNOISE_SELF = 0,
	PNOISE_WEAPON = 1,
	PNOISE_IMPACT = 2,
};

typedef enum damage_t{
	DAMAGE_NO,
	DAMAGE_YES,	// will take damage if hit
	DAMAGE_AIM	// auto targeting recognizes this
}damage_t;
typedef enum weaponstate_t{
	WEAPON_READY, 
	WEAPON_ACTIVATING,
	WEAPON_DROPPING,
	WEAPON_FIRING
}weaponstate_t;
typedef enum ammo_t{
	AMMO_BULLETS,
	AMMO_SHELLS,
	AMMO_ROCKETS,
	AMMO_GRENADES,
	AMMO_CELLS,
	AMMO_SLUGS
}ammo_t;

/* edict->movetype values */
typedef enum movetype_t{
	MOVETYPE_NONE,	// never moves
	MOVETYPE_NOCLIP,	// origin and angles change with no interaction
	MOVETYPE_PUSH,	// no clip to world, push on box contact
	MOVETYPE_STOP,	// no clip to world, stops on box contact
	MOVETYPE_WALK,	// gravity
	MOVETYPE_STEP,	// gravity, special edge handling
	MOVETYPE_FLY,
	MOVETYPE_TOSS,	// gravity
	MOVETYPE_FLYMISSILE,	// extra size to monsters
	MOVETYPE_BOUNCE
}movetype_t;

struct gitem_armor_t{
	int base_count;
	int max_count;
	float normal_protection;
	float energy_protection;
	int armor;
};

enum{
	/* gitem_t->flags */
	IT_WEAPON = 1<<0,		// use makes active weapon
	IT_AMMO = 1<<1,
	IT_ARMOR = 1<<2,
	IT_STAY_COOP = 1<<3,
	IT_KEY = 1<<4,
	IT_POWERUP = 1<<5,
	/* gitem_t->weapmodel for weapons indicates model index */
	WEAP_BLASTER = 1, 
	WEAP_SHOTGUN = 2, 
	WEAP_SUPERSHOTGUN = 3, 
	WEAP_MACHINEGUN = 4, 
	WEAP_CHAINGUN = 5, 
	WEAP_GRENADES = 6, 
	WEAP_GRENADELAUNCHER = 7, 
	WEAP_ROCKETLAUNCHER = 8, 
	WEAP_HYPERBLASTER = 9, 
	WEAP_RAILGUN = 10,
	WEAP_BFG = 11
};
struct gitem_t{
	char *classname;	// spawning name
	qboolean	(*pickup)(edict_t *, edict_t *);
	void	(*use)(edict_t *, gitem_t *);
	void	(*drop)(edict_t *, gitem_t *);
	void	(*weaponthink)(edict_t *);
	char *pickup_sound;
	char *world_model;
	int world_model_flags;
	char *view_model;
	/* client side info */
	char *icon;
	char *pickup_name;	// for printing on pickup
	int count_width;	// number of digits to display by icon
	int quantity;	// for ammo how much, for weapons how much is used per shot
	char *ammo;	// for weapons
	int flags;	// IT_* flags
	int weapmodel;	// weapon model index (for weapons)
	void *info;
	int tag;
	/* string of all models, sounds, and images this item will use */
	char *precaches;
};
extern gitem_t itemlist[];

/* this structure is left intact through an entire game it should be
 * initialized at dll load time, and read/written to the server.ssv file for
 * savegames */
struct game_locals_t{
	char helpmessage1[512];
	char helpmessage2[512];
	/* flash F1 icon if non 0, play sound and increment only if 1, 2, or 3 */
	int helpchanged;
	gclient_t *clients;	// [maxclients]
	/* can't store spawnpoint in level, because it would get overwritten
	 * by the savegame restore */
	char spawnpoint[512];	// needed for coop respawns
	/* store latched cvars here that we want to get at often */
	int maxclients;
	int maxentities;
	/* cross level triggers */
	int serverflags;
	int num_items;
	qboolean autosaved;
};
extern game_locals_t game;

/* this structure is cleared as each map is entered; it is read/written to the
 * level.sav file for savegames */
struct level_locals_t{
	int framenum;
	float time;
	char level_name[MAX_QPATH];	// the descriptive name (Outer Base, etc)
	char mapname[MAX_QPATH];	// the server name (base1, etc)
	char nextmap[MAX_QPATH];	// go here when fraglimit is hit
	/* intermission state */
	float intermissiontime;	// time the intermission was started
	char *changemap;
	int exitintermission;
	vec3_t intermission_origin;
	vec3_t intermission_angle;
	edict_t *sight_client;	// changed once each frame for coop games
	edict_t *sight_entity;
	int sight_entity_framenum;
	edict_t *sound_entity;
	int sound_entity_framenum;
	edict_t *sound2_entity;
	int sound2_entity_framenum;
	int pic_health;
	int total_secrets;
	int found_secrets;
	int total_goals;
	int found_goals;
	int total_monsters;
	int killed_monsters;
	edict_t *current_entity;	// entity running from G_RunFrame
	int body_que;	// dead bodies
	int power_cubes;	// ugly necessity for coop
};
extern level_locals_t level;

/* spawn_temp_t is only used to hold entity field values that can be set from
 * the editor, but aren't actualy present in edict_t during gameplay */
struct spawn_temp_t{
	/* world vars */
	char *sky;
	float skyrotate;
	vec3_t skyaxis;
	char *nextmap;

	int lip;
	int distance;
	int height;
	char *noise;
	float pausetime;
	char *item;
	char *gravity;
	float minyaw;
	float maxyaw;
	float minpitch;
	float maxpitch;
};
extern spawn_temp_t st;

struct moveinfo_t{
	/* fixed data */
	vec3_t start_origin;
	vec3_t start_angles;
	vec3_t end_origin;
	vec3_t end_angles;
	int sound_start;
	int sound_middle;
	int sound_end;
	float accel;
	float speed;
	float decel;
	float distance;
	float wait;
	/* state data */
	int state;
	vec3_t dir;
	float current_speed;
	float move_speed;
	float next_speed;
	float remaining_distance;
	float decel_distance;
	void	(*endfunc)(edict_t *);
};

struct mframe_t{
	void	(*aifunc)(edict_t *, float);
	float dist;
	void	(*thinkfunc)(edict_t *);
};

struct mmove_t{
	int firstframe;
	int lastframe;
	mframe_t *frame;
	void	(*endfunc)(edict_t *);
};

struct monsterinfo_t{
	mmove_t *currentmove;
	int aiflags;
	int nextframe;
	float scale;
	void	(*stand)(edict_t *);
	void	(*idle)(edict_t *);
	void	(*search)(edict_t *);
	void	(*walk)(edict_t *);
	void	(*run)(edict_t *);
	void	(*dodge)(edict_t *, edict_t *, float);
	void	(*attack)(edict_t *);
	void	(*melee)(edict_t *);
	void	(*sight)(edict_t *, edict_t *);
	qboolean	(*checkattack)(edict_t *);
	float pausetime;
	float attack_finished;
	vec3_t saved_goal;
	float search_time;
	float trail_time;
	vec3_t last_sighting;
	int attack_state;
	int lefty;
	float idle_time;
	int linkcount;
	int power_armor_type;
	int power_armor_power;
};

extern int sm_meat_index;
extern int snd_fry;

/* means of death */
enum{
	MOD_UNKNOWN = 0,
	MOD_BLASTER = 1,
	MOD_SHOTGUN = 2,
	MOD_SSHOTGUN = 3,
	MOD_MACHINEGUN = 4,
	MOD_CHAINGUN = 5,
	MOD_GRENADE = 6,
	MOD_G_SPLASH = 7,
	MOD_ROCKET = 8,
	MOD_R_SPLASH = 9,
	MOD_HYPERBLASTER = 10,
	MOD_RAILGUN = 11,
	MOD_BFG_LASER = 12,
	MOD_BFG_BLAST = 13,
	MOD_BFG_EFFECT = 14,
	MOD_HANDGRENADE = 15,
	MOD_HG_SPLASH = 16,
	MOD_WATER = 17,
	MOD_SLIME = 18,
	MOD_LAVA = 19,
	MOD_CRUSH = 20,
	MOD_TELEFRAG = 21,
	MOD_FALLING = 22,
	MOD_SUICIDE = 23,
	MOD_HELD_GRENADE = 24,
	MOD_EXPLOSIVE = 25,
	MOD_BARREL = 26,
	MOD_BOMB = 27,
	MOD_EXIT = 28,
	MOD_SPLASH = 29,
	MOD_TARGET_LASER = 30,
	MOD_TRIGGER_HURT = 31,
	MOD_HIT = 32,
	MOD_TARGET_BLASTER = 33,
	MOD_FRIENDLY_FIRE = 1<<27
};
extern int meansOfDeath;

enum{
	/* item spawnflags */
	ITEM_TRIGGER_SPAWN = 1<<0,
	ITEM_NO_TOUCH = 1<<1,
	/* 6 bits reserved for editor flags; 8 bits used as power cube id bits
	 * for coop games */
	DROPPED_ITEM = 1<<16,
	DROPPED_PLAYER_ITEM = 1<<17,
	ITEM_TARGETS_USED = 1<<18,
	/* fields are needed for spawning from the entity string and saving or
	 * loading games */
	FFL_SPAWNTEMP = 1,
	FFL_NOSPAWN = 2
};
typedef enum fieldtype_t{
	F_INT, 
	F_FLOAT,
	F_LSTRING,	// string on disk, pointer in memory, TAG_LEVEL
	F_GSTRING,	// string on disk, pointer in memory, TAG_GAME
	F_VECTOR,
	F_ANGLEHACK,
	F_EDICT,	// index on disk, pointer in memory
	F_ITEM,	// index on disk, pointer in memory
	F_CLIENT,	// index on disk, pointer in memory
	F_FUNCTION,
	F_MMOVE,
	F_IGNORE
}fieldtype_t;
struct field_t{
	char *name;
	int ofs;
	fieldtype_t type;
	int flags;
};
extern field_t fields[];

enum{
	/* damage flags */
	DAMAGE_RADIUS = 1<<0,	// indirect dmg
	DAMAGE_NO_ARMOR = 1<<1,	// not protected by armour
	DAMAGE_ENERGY = 1<<2,	// is from an energy based weapon
	DAMAGE_NO_KNOCKBACK = 1<<3,	// do not affect velocity, just view angles
	DAMAGE_BULLET = 1<<4,	// is from a bullet (used for ricochets)
	/* no effect from armor, shields, invulnerability, and godmode */
	DAMAGE_NO_PROTECTION = 1<<5,

	DEFAULT_BULLET_HSPREAD = 300,
	DEFAULT_BULLET_VSPREAD = 500,
	DEFAULT_SHOTGUN_HSPREAD = 1000,
	DEFAULT_SHOTGUN_VSPREAD = 500,
	DEFAULT_DEATHMATCH_SHOTGUN_COUNT = 12,
	DEFAULT_SHOTGUN_COUNT = 12,
	DEFAULT_SSHOTGUN_COUNT = 20,

	/* client_t->anim_priority */
	ANIM_BASIC = 0,	// stand / run
	ANIM_WAVE = 1,
	ANIM_JUMP = 2,
	ANIM_PAIN = 3,
	ANIM_ATTACK = 4,
	ANIM_DEATH = 5,
	ANIM_REVERSE = 6
};

/* client data that stays across multiple level loads */
struct client_persistant_t{
	char userinfo[MAX_INFO_STRING];
	char netname[16];
	int hand;
	/* a loadgame will leave valid entities that just don't have a connection yet */
	qboolean connected;
	/* values saved and restored from edicts when changing levels */
	int health;
	int max_health;
	int savedFlags;
	int selected_item;
	int inventory[MAX_ITEMS];
	int max_bullets;
	int max_shells;
	int max_rockets;
	int max_grenades;
	int max_cells;
	int max_slugs;
	gitem_t *weapon;
	gitem_t *lastweapon;
	int power_cubes;	// used for tracking the cubes in coop games
	int score;	// for calculating total unit score in coop games
	int game_helpchanged;
	int helpchanged;
	qboolean spectator;	// client is a spectator
};
/* client data that stays across deathmatch respawns */
struct client_respawn_t{
	client_persistant_t coop_respawn;	// assigned to client->pers on a respawn
	int enterframe;	// level.framenum the client entered the game
	int score;	// frags, etc
	vec3_t cmd_angles;	// angles sent over in the last command
	qboolean spectator;	// client is a spectator
};
/* this structure is cleared on each PutClientInServer(), except for .pers */
struct gclient_t{
	/* known to server */
	player_state_t ps;	// communicated by server to clients
	int ping;

	/* the server expects the first part of gclient_s to be a
	 * player_state_t but the rest of it is opaque
	 * DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER EXPECTS THE FIELDS IN
	 * THAT ORDER! */

	client_persistant_t pers;
	client_respawn_t resp;
	pmove_state_t old_pmove;	// for detecting out-of-pmove changes
	qboolean showscores;	// set layout stat
	qboolean showinventory;	// set layout stat
	qboolean showhelp;
	qboolean showhelpicon;
	int ammo_index;
	int buttons;
	int oldbuttons;
	int latched_buttons;
	qboolean weapon_thunk;
	gitem_t *newweapon;

	/* sum up damage over an entire frame, so shotgun blasts give a single
	 * big kick */
	int damage_armor;	// damage absorbed by armor
	int damage_parmor;	// damage absorbed by power armor
	int damage_blood;	// damage taken out of health
	int damage_knockback;	// impact damage
	vec3_t damage_from;	// origin for vector calculation
	float killer_yaw;	// when dead, look at killer
	weaponstate_t weaponstate;
	vec3_t kick_angles;	// weapon kicks
	vec3_t kick_origin;
	float v_dmg_roll;	// damage kicks
	float v_dmg_pitch;
	float v_dmg_time;
	float fall_time;	// for view drop on fall
	float fall_value;
	float damage_alpha;
	float bonus_alpha;
	vec3_t damage_blend;
	vec3_t v_angle;	// aiming direction so off-ground doesn't change it
	float bobtime;
	vec3_t oldviewangles;
	vec3_t oldvelocity;
	float next_drown_time;
	int old_waterlevel;
	int breather_sound;
	int machinegun_shots;	// for weapon raising
	int anim_end;
	int anim_priority;
	qboolean anim_duck;
	qboolean anim_run;
	/* powerup timers */
	float quad_framenum;
	float invincible_framenum;
	float breather_framenum;
	float enviro_framenum;
	qboolean grenade_blew_up;
	float grenade_time;
	int silencer_shots;
	int weapon_sound;
	float pickup_msg_time;
	float flood_locktill;	// locked from talking
	float flood_when[10];	// when messages were said
	int flood_whenhead;	// head pointer for when said
	float respawn_time;	// can respawn when time > this
	edict_t *chase_target;	// player we are chasing
	qboolean update_chase;	// need to update chase info?
};
struct edict_t{
	entity_state_t s;
	gclient_t *client;	// nil if not a player
	qboolean inuse;
	int linkcount;
	// FIXME: move these fields to a server private sv_entity_t
	link_t area;	// linked to a division node or leaf
	int num_clusters;	// if -1, use headnode instead
	int clusternums[MAX_ENT_CLUSTERS];
	int headnode;	// unused if num_clusters != -1
	int areanum;
	int areanum2;
	int svflags;	// SCF_NOCLIENT, SVF_DEADMONSTER, SVF_MONSTER, etc.
	vec3_t mins;
	vec3_t maxs;
	vec3_t absmin;
	vec3_t absmax;
	vec3_t size;
	solid_t solid;
	int clipmask;
	edict_t *owner;

	/* DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER EXPECTS THE FIELDS IN
	 * THAT ORDER! */

	int movetype;
	int flags;
	char *model;
	float freetime;	// sv.time when the object was freed
	/* only used locally in game, not by server */
	char *message;
	char *classname;
	int spawnflags;
	float timestamp;
	float angle;	// set in qe3, -1 = up, -2 = down
	char *target;
	char *targetname;
	char *killtarget;
	char *team;
	char *pathtarget;
	char *deathtarget;
	char *combattarget;
	edict_t *target_ent;
	float speed;
	float accel;
	float decel;
	vec3_t movedir;
	vec3_t pos1;
	vec3_t pos2;
	vec3_t velocity;
	vec3_t avelocity;
	int mass;
	float air_finished;
	/* per entity gravity multiplier (1.0 is normal); use for lowgrav
	 * artifact, flares */
	float gravity;
	edict_t *goalentity;
	edict_t *movetarget;
	float yaw_speed;
	float ideal_yaw;
	float nextthink;
	void	(*prethink)(edict_t *);
	void	(*think)(edict_t *);
	void	(*blocked)(edict_t *, edict_t *);	//move to moveinfo?
	void	(*touch)(edict_t *, edict_t *, cplane_t *, csurface_t *);
	void	(*use)(edict_t *, edict_t *, edict_t *);
	void	(*pain)(edict_t *, edict_t *, float, int);
	void	(*die)(edict_t *, edict_t *, edict_t *, int, vec3_t);
	float touch_debounce_time;	// are all these legit?  do we need more/less of them?
	float pain_debounce_time;
	float damage_debounce_time;
	float fly_sound_debounce_time;	//move to clientinfo
	float last_move_time;
	int health;
	int max_health;
	int gib_health;
	int deadflag;
	qboolean show_hostile;
	float powerarmor_time;
	char *map;	// target_changelevel
	int viewheight;	// height above origin where eyesight is determined
	int takedamage;
	int dmg;
	int radius_dmg;
	float dmg_radius;
	int sounds;	//make this a spawntemp var?
	int count;
	edict_t *chain;
	edict_t *enemy;
	edict_t *oldenemy;
	edict_t *activator;
	edict_t *groundentity;
	int groundentity_linkcount;
	edict_t *teamchain;
	edict_t *teammaster;
	edict_t *mynoise;	// can go in client only
	edict_t *mynoise2;
	int noise_index;
	int noise_index2;
	float volume;
	float attenuation;
	/* timing variables */
	float wait;
	float delay;	// before firing targets
	float random;
	float teleport_time;
	int watertype;
	int waterlevel;
	vec3_t move_origin;
	vec3_t move_angles;
	/* move this to clientinfo? */
	int light_level;
	int style;	// also used as areaportal number
	gitem_t *item;	// for bonus items
	/* common data blocks */
	moveinfo_t moveinfo;
	monsterinfo_t monsterinfo;
};
extern edict_t *g_edicts;

#define WORLD	(&g_edicts[0])

#define	ITEM_INDEX(x)	((x)-itemlist)
#define	FOFS(x)	(uintptr)&(((edict_t *)0)->x)
#define	STOFS(x)	(uintptr)&(((spawn_temp_t *)0)->x)
#define	LLOFS(x)	(uintptr)&(((level_locals_t *)0)->x)
#define	CLOFS(x)	(uintptr)&(((gclient_t *)0)->x)
#define qrandom()	((rand () & 0x7fff) / ((float)0x7fff))	/* >_< arrrrggghh */
#define crandom()	(2.0 * (qrandom() - 0.5))

void	Cmd_Help_f(edict_t *);
void	Cmd_Score_f(edict_t *);
void	PrecacheItem(gitem_t *);
void	InitItems(void);
void	SetItemNames(void);
gitem_t*	FindItem(char *);
gitem_t*	FindItemByClassname(char *);
edict_t*	Drop_Item(edict_t *, gitem_t *);
void	SetRespawn(edict_t *, float);
void	ChangeWeapon(edict_t *);
void	SpawnItem(edict_t *, gitem_t *);
void	Think_Weapon(edict_t *);
int	ArmorIndex(edict_t *);
int	PowerArmorType(edict_t *);
gitem_t*	GetItemByIndex(int);
qboolean	Add_Ammo(edict_t *, gitem_t *, int);
void	Touch_Item(edict_t *, edict_t *, cplane_t *, csurface_t *);
qboolean	KillBox(edict_t *);
void	G_ProjectSource(vec3_t, vec3_t, vec3_t, vec3_t, vec3_t);
edict_t*	G_Find(edict_t *, int, char *);
edict_t*	findradius(edict_t *, vec3_t, float);
edict_t*	G_PickTarget(char *);
void	G_UseTargets(edict_t *, edict_t *);
void	G_SetMovedir(vec3_t, vec3_t);
void	G_InitEdict(edict_t *);
edict_t*	G_Spawn(void);
void	G_FreeEdict(edict_t *);
void	G_TouchTriggers(edict_t *);
void	G_TouchSolids(edict_t *);
char*	G_CopyString(char *);
float*	tv(float, float, float);
char*	vtos(vec3_t);
float	vectoyaw(vec3_t);
void	vectoangles(vec3_t, vec3_t);
qboolean	OnSameTeam(edict_t *, edict_t *);
qboolean	CanDamage(edict_t *, edict_t *);
void	T_Damage(edict_t *, edict_t *, edict_t *, vec3_t, vec3_t, vec3_t, int, int, int, int);
void	T_RadiusDamage(edict_t *, edict_t *, float, edict_t *, float, int);
void	monster_fire_bullet(edict_t *, vec3_t, vec3_t, int, int, int, int, int);
void	monster_fire_shotgun(edict_t *, vec3_t, vec3_t, int, int, int, int, int, int);
void	monster_fire_blaster(edict_t *, vec3_t, vec3_t, int, int, int, int);
void	monster_fire_grenade(edict_t *, vec3_t, vec3_t, int, int, int);
void	monster_fire_rocket(edict_t *, vec3_t, vec3_t, int, int, int);
void	monster_fire_railgun(edict_t *, vec3_t, vec3_t, int, int, int);
void	monster_fire_bfg(edict_t *, vec3_t, vec3_t, int, int, int, float, int);
void	M_droptofloor(edict_t *);
void	monster_think(edict_t *);
void	walkmonster_start(edict_t *);
void	swimmonster_start(edict_t *);
void	flymonster_start(edict_t *);
void	AttackFinished(edict_t *, float);
void	monster_death_use(edict_t *);
void	M_CatagorizePosition(edict_t *);
qboolean	M_CheckAttack(edict_t *);
void	M_FlyCheck(edict_t *);
void	M_CheckGround(edict_t *);
void	ThrowHead(edict_t *, char *, int, int);
void	ThrowClientHead(edict_t *, int);
void	ThrowGib(edict_t *, char *, int, int);
void	BecomeExplosion1(edict_t *);
void	AI_SetSightClient(void);
void	ai_stand(edict_t *, float);
void	ai_move(edict_t *, float);
void	ai_walk(edict_t *, float);
void	ai_turn(edict_t *, float);
void	ai_run(edict_t *, float);
void	ai_charge(edict_t *, float);
int	range(edict_t *, edict_t *);
void	FoundTarget(edict_t *);
qboolean	infront(edict_t *, edict_t *);
qboolean	visible(edict_t *, edict_t *);
qboolean	FacingIdeal(edict_t *);
void	ThrowDebris(edict_t *, char *, float, vec3_t);
qboolean	fire_hit(edict_t *, vec3_t, int, int);
void	fire_bullet(edict_t *, vec3_t, vec3_t, int, int, int, int, int);
void	fire_shotgun(edict_t *, vec3_t, vec3_t, int, int, int, int, int, int);
void	fire_blaster(edict_t *, vec3_t, vec3_t, int, int, int, qboolean);
void	fire_grenade(edict_t *, vec3_t, vec3_t, int, int, float, float);
void	fire_grenade2(edict_t *, vec3_t, vec3_t, int, int, float, float, qboolean);
void	fire_rocket(edict_t *, vec3_t, vec3_t, int, int, float, int);
void	fire_rail(edict_t *, vec3_t, vec3_t, int, int);
void	fire_bfg(edict_t *, vec3_t, vec3_t, int, int, float);
void	PlayerTrail_Init(void);
void	PlayerTrail_Add(vec3_t);
void	PlayerTrail_New(vec3_t);
edict_t*	PlayerTrail_PickFirst(edict_t *);
edict_t*	PlayerTrail_PickNext(edict_t *);
edict_t*	PlayerTrail_LastSpot(void);
void	respawn(edict_t *);
void	BeginIntermission(edict_t *);
void	PutClientInServer(edict_t *);
void	InitClientPersistant(gclient_t *);
void	InitClientResp(gclient_t *);
void	InitBodyQue(void);
void	ClientBeginServerFrame(edict_t *);
void	player_pain(edict_t *, edict_t *, float, int);
void	player_die(edict_t *, edict_t *, edict_t *, int, vec3_t);
void	ServerCommand(void);
qboolean	SV_FilterPacket(char *);
void	ClientEndServerFrame(edict_t *);
void	MoveClientToIntermission(edict_t *);
void	G_SetStats(edict_t *);
void	G_SetSpectatorStats(edict_t *);
void	G_CheckChaseStats(edict_t *);
void	ValidateSelectedItem(edict_t *);
void	DeathmatchScoreboardMessage(edict_t *, edict_t *);
void	PlayerNoise(edict_t *, vec3_t, int);
qboolean	M_CheckBottom(edict_t *);
qboolean	M_walkmove(edict_t *, float, float);
void	M_MoveToGoal(edict_t *, float);
void	M_ChangeYaw(edict_t *);
void	G_RunEntity(edict_t *);
void	SaveClientData(void);
void	FetchClientEntData(edict_t *);
void	UpdateChaseCam(edict_t *);
void	ChaseNext(edict_t *);
void	ChasePrev(edict_t *);
void	GetChaseTarget(edict_t *);
