#include <u.h>
#include <libc.h>
#include <stdio.h>
#include "dat.h"
#include "fns.h"

int
mkdir(char *path)
{
	int fd;

	if(access(path, AEXIST) == 0)
		return 0;
	if((fd = create(path, OREAD, DMDIR|0777)) < 0){
		fprint(2, "mkdir: %r\n");
		return -1;
	}
	close(fd);
	return 0;
}
