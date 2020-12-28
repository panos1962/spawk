#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <cht.h>

int cht_fnew(char *s)
{
	struct stat st;

	if (s == (char *)0) {
		cht_error("fnew: missing filename");
		return 0;	
	}

	if (!strcmp(s, "/dev/null"))
		return 0;

	if (!stat(s, &st)) {
		cht_error(NULL);
		fprintf(stderr, "%s: file exists\n", s);
		return 1;
	}

	if (errno != ENOENT) {
		cht_error(NULL);
		perror(s);
		return 1;
	}

	return 0;
}
