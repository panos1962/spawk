#include "sparc.h"
#include <unistd.h>
#include <string.h>

static char *set_progname(char *);
static void usage(void);

int main(int argc, char *argv[]) {
	int c;
	int errs = 0;
	extern int optind;
	extern char *optarg;
	extern int yyparse(void);

	argv[0] = set_progname(argv[0]);
	while ((c = getopt(argc, argv, "")) != EOF) {
		switch (c) {
		case '?':
			errs++;
			break;
		}
	}

	if (errs)
	usage();

	argc -= optind;
	argv += optind;

	if (!argc)
	yyparse();

	else for (; argc > 0; argc--, argv++) {
		source = *argv;
		lineno = 1;
		if (freopen(source, "r", stdin) == NULL) {
			sparc_error(NULL);
			perror(source);
			errs++;
		}
		else
			yyparse();
	}

	if (errs) {
		sparc_error("errors encountered (compilation failed)");
		exit(EXIT_SYNTAX);
	}

	exit(0);
}

static char *set_progname(char *s) {
	if (!s)
	return progname;

	if (!(progname = strrchr(s, '/')))
	progname = s;

	return progname;
}

static void usage(void) {
	fprintf(stderr, "usage: %s [files]\n", progname);
	exit(EXIT_USAGE);
}
