/*
 * flushbsd.c --- This routine flushes the disk buffers for a disk
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
const char *progname;

static void usage(NOARGS)
{
	fprintf(stderr, "Usage: %s disk\n", progname);
	exit(1);
}	
	
int main(int argc, char **argv)
{
	int	fd;
	
	progname = argv[0];
	if (argc != 2)
		usage();

	fd = open(argv[1], O_RDONLY, 0);
	if (fd < 0) {
		perror("open");
		exit(1);
	}
	fsync(fd);
	fsync(fd);
	
	return 0;
}
