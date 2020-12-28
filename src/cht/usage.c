/*
** η function cht_usage χρησιμποιείται για να τυπώσει
** τη σωστή χρήση της εντολής (προγράμματος).
** Καλείται συνήθως στην main function του προγράμματος.
*/

#include <stdlib.h>
#include <stdio.h>
#include <cht.h>

void cht_usage(char *s, int err)
{
	fputs("usage: ", stderr);
	fputs(cht_progname, stderr);

	if (s != (char *)0) {
		putc(' ', stderr);
		fputs(s, stderr);
	}

	putc('\n', stderr);
	exit(err);
}
