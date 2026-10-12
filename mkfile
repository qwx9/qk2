GAMES=\
	baseq2\
	ctf\
	rogue\
	xatrix\
	crbot\

install safeinstall clean nuke:V:
	for(i in $GAMES) @{
		rfork n
		bind -bc $i .
		mk $target
	}
