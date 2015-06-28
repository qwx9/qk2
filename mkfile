</$objtype/mkfile

BIN=$home/bin/$objtype

# change this to build/load a different game "dll"
<mk.baseq2

OFILES=\
	cl_cin.$O\
	cl_ents.$O\
	cl_fx.$O\
	cl_newfx.$O\
	cl_input.$O\
	cl_inv.$O\
	cl_main.$O\
	cl_parse.$O\
	cl_pred.$O\
	cl_tent.$O\
	cl_scrn.$O\
	cl_view.$O\
	console.$O\
	keys.$O\
	menu.$O\
	snd_dma.$O\
	snd_mem.$O\
	snd_mix.$O\
	qmenu.$O\
	sv_ccmds.$O\
	sv_ents.$O\
	sv_game.$O\
	sv_init.$O\
	sv_main.$O\
	sv_send.$O\
	sv_user.$O\
	sv_world.$O\
	cmd.$O\
	cmodel.$O\
	common.$O\
	crc.$O\
	cvar.$O\
	files.$O\
	md4.$O\
	net_chan.$O\
	pmove.$O\
	cd.$O\
	in.$O\
	snd.$O\
	sys.$O\
	udp.$O\
	vid.$O\
	vmenu.$O\
	r_aclip.$O\
	r_alias.$O\
	r_bsp.$O\
	r_draw.$O\
	r_edge.$O\
	r_image.$O\
	r_light.$O\
	r_main.$O\
	r_misc.$O\
	r_model.$O\
	r_part.$O\
	r_poly.$O\
	r_polyse.$O\
	r_rast.$O\
	r_scan.$O\
	r_sprite.$O\
	r_surf.$O\
	$GMOFILES\

HFILES=\
	adivtab.h\
	anorms.h\
	dat.h\
	fns.h\
	rand1k.h\
	$GMHFILES\

# FIXME
CLEANFILES=$OFILES */*.acid

</sys/src/cmd/mkone

%.$O:	%.c
	$CC -o $target $CFLAGS $stem.c
