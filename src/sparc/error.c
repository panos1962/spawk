#include "sparc.h"

void sparc_error(char *s) {
	fprintf(stderr, "%s: ", progname);

	if (!s)
	return;

	fprintf(stderr, "%s\n", s);
	fflush(stderr);
}

void sparc_fatal(char *s, int err) {
	if (!s)
	s = "fatal error";

	sparc_error(s);
	exit(err);
}

void sparc_nomemory(char *s) {
	sparc_error(NULL);

	if (s)
	fprintf(stderr, "%s: ", s);

	fprintf(stderr, "out of memory\n");
	exit(EXIT_MEMORY);
}

void sparc_locerror(char *s) {
	sparc_error(NULL);

	if (source)
	fprintf(stderr, "%s: ", source);

	fprintf(stderr, "line %u: ", lineno);

	if (!s)
	return;

	fprintf(stderr, "%s\n", s);
	fflush(stderr);
}

void sparc_locfatal(char *s, const int err) {
	if (!s)
	s = "compiler error";

	sparc_fatal(s, err);
}

int yyerror(char *s) {
	sparc_locerror(s);
	error_count++;
}
