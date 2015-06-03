</$objtype/mkfile

BIN=$home/bin/$objtype
TARG=quake2

# change this to build/load a different game "dll"
<mk.baseq2

OFILES=\
	client/cl_cin.$O\
	client/cl_ents.$O\
	client/cl_fx.$O\
	client/cl_newfx.$O\
	client/cl_input.$O\
	client/cl_inv.$O\
	client/cl_main.$O\
	client/cl_parse.$O\
	client/cl_pred.$O\
	client/cl_tent.$O\
	client/cl_scrn.$O\
	client/cl_view.$O\
	client/console.$O\
	client/keys.$O\
	client/menu.$O\
	client/snd_dma.$O\
	client/snd_mem.$O\
	client/snd_mix.$O\
	client/qmenu.$O\
	server/sv_ccmds.$O\
	server/sv_ents.$O\
	server/sv_game.$O\
	server/sv_init.$O\
	server/sv_main.$O\
	server/sv_send.$O\
	server/sv_user.$O\
	server/sv_world.$O\
	qcommon/cmd.$O\
	qcommon/cmodel.$O\
	qcommon/common.$O\
	qcommon/crc.$O\
	qcommon/cvar.$O\
	qcommon/files.$O\
	qcommon/md4.$O\
	qcommon/net_chan.$O\
	qcommon/pmove.$O\
	plan9/cd.$O\
	plan9/in.$O\
	plan9/menu.$O\
	plan9/misc.$O\
	plan9/snd.$O\
	plan9/sys.$O\
	plan9/udp.$O\
	plan9/vid.$O\
	ref/r_aclip.$O\
	ref/r_alias.$O\
	ref/r_bsp.$O\
	ref/r_draw.$O\
	ref/r_edge.$O\
	ref/r_image.$O\
	ref/r_light.$O\
	ref/r_main.$O\
	ref/r_misc.$O\
	ref/r_model.$O\
	ref/r_part.$O\
	ref/r_poly.$O\
	ref/r_polyse.$O\
	ref/r_rast.$O\
	ref/r_scan.$O\
	ref/r_sprite.$O\
	ref/r_surf.$O\
	$GMOFILES\

HFILES=\
	anorms.h\
	q_shared.h\
	client/cdaudio.h\
	client/client.h\
	client/console.h\
	client/input.h\
	client/keys.h\
	client/qmenu.h\
	client/ref.h\
	client/screen.h\
	client/snd_loc.h\
	client/sound.h\
	client/vid.h\
	server/server.h\
	qcommon/crc.h\
	qcommon/qcommon.h\
	qcommon/qfiles.h\
	ref/adivtab.h\
	ref/r_local.h\
	ref/rand1k.h\
	$GMHFILES\

# FIXME
CLEANFILES=$OFILES */*.acid

</sys/src/cmd/mkone

%.$O:	%.c
	$CC -o $target $CFLAGS $stem.c
