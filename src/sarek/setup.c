#include "sarek.h"

static char *extract_string(char *, char **);
static char *strsave(char *, char *);
static void check_deficient(char *);

void setup(char *message) {
	if (sarek_result)
	sarek_fatal("unexpected \"setup\" message", EXIT_SETUP);

	if (sarek_connection)
	mysql_close(sarek_connection);

	sarek_connection = NULL;

	message = extract_string(message, &sarek_user);
	message = extract_string(message, &sarek_password);
	message = extract_string(message, &sarek_database);
	message = extract_string(message, &sarek_host);
	message = extract_string(message, &sarek_charset);
	message = extract_string(message, &sarek_null);

	switch (*message) {
	case '0':
		break;
	case '1':
		debug_mode();
		break;
	default:
		sarek_fatal("invalid setup debug code", EXIT_SETUP);
		break;
	}
}

static char *extract_string(char *message, char **s) {
	char *p;

	for (p = message; *p; p++) {
		if (*p == '\t') {
			*s = strsave(message, p);
			return p + 1;
		}
	}

	sarek_fatal("deficient setup information", EXIT_SETUP);
}

static char *strsave(char *from, char *to) {
	unsigned int l = to - from + 1;
	char *p;
	char *q;

	if (!(p = malloc(l)))
	sarek_fatal("out of memory", EXIT_MEMORY);

	for (q = p; from < to;)
	*q++ = *from++;

	*q = '\0';

	return p;
}
