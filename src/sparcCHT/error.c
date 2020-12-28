#include "sparc.h"

void warning(char *s)
{
	cht_error(STR_NULL);
	if (source != STR_NULL)
		fprintf(stderr, "%s: ", source);

	fprintf(stderr, "line %d: ", lineno);
	if (s == STR_NULL)
		return;

	fputs(s, stderr);
	putc('\n', stderr);
	fflush(stderr);
}

int yyerror(char *s)
{
	warning(s);
	n_errs++;
}

void fatal(char *s, const int n)
{
	warning(s);
	exit(n);
}
