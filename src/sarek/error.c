#include "sarek.h"

void sarek_error(const char *msg) {
	fprintf(stderr, "%s: ", sarek_progname);

	if (msg)
	fputs(msg, stderr);
}

void sarek_fatal(const char *msg, int status) {
	if (!msg)
	msg = "fatal error";

	sarek_error(msg);
	putc('\n', stderr);
	exit(status);
}

void sarek_sqlerror(char *msg) {
	fprintf(stderr, "%s: ", sarek_progname);

	if (msg)
	fprintf(stderr, "%s: ", msg);

	fprintf(stderr, "%d: %s\n", mysql_errno(sarek_connection),
		mysql_error(sarek_connection));
}

void sarek_sqlfatal(char *msg, int status) {
	sarek_sqlerror(msg);
	exit(status);
}
