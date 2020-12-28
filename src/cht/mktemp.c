#include <stdlib.h>
#include <stdio.h>
#include <cht.h>

char *cht_mktemp(char *s, const int err)
{
	int fd;
	register char *p;
	register int cnt_X;
	static char buf[64];

	if (s == (char *)NULL) {
		strcpy(buf, "/tmp/tmpXXXXXX");
		s = buf;
	}

	for (cnt_X = 0, p = s; *p; p++) {
		if (*p == 'X')
			cnt_X++;
		else
			cnt_X = 0;
	}

	if ((cnt_X != 6) || ((fd = mkstemp(s)) == -1) || close(fd)) {
		cht_error("cht_mktemp");
		perror(s);
		exit(err);
	}

	return s;
}
