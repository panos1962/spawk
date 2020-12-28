#include "sarek.h"
#include <ctype.h>
#include <string.h>

static char *trim_message(char *);

char *get_message(void) {
	char *p = sarek_message;
	int c;

	while ((c = getchar()) != EOF) {
		if (c == sarek_eom)
		return trim_message(p);

		// Ignore left side white characters.

		if ((p == sarek_message) && isspace(c))
		continue;

		if ((p - sarek_message) >= MAX_MESSAGE_LEN)
		sarek_fatal("huge message", EXIT_HUGE_MESSAGE);

		*p++ = c;
	}

	return trim_message(p);
}

// Trim right side newline characters

static char *trim_message(char *p) {
	if (p <= sarek_message)
	return NULL;

	while (--p >= sarek_message) {
		switch (*p) {
		case '\n':
			break;
		default:
			p++;
			goto OK;
		}
	}

OK:
	*p = '\0';
	return sarek_message;
}
