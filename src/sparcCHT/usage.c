#include "sparc.h"
#include <string.h>

void usage(void)
{
	char *ask = (strcmp(cht_progname, "art") ? "chtawk" : "ask");

	fprintf(stderr, "usage: %s -c [-b] [-O] [-S] [-o object] "
		"sources...\n"
		"       %s [-R id] [-r] [-S] object "
		"[files...] [ -- %s args... ]\n"
		"       %s [-R id] [-S] -s source "
		"[files...] [ -- -- %s args... ]\n",
		cht_progname, cht_progname, ask, cht_progname, ask);
	exit(EXIT_USAGE);
}
