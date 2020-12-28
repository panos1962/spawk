#include "sarek.h"
#include <wchar.h>

static void colval(char *);

void print_row(void) {
	MYSQL_ROW row;
	int i;

	if (sarek_debug)
	fprintf(stderr, "FETCH ROW\n");

	if (!sarek_result)
	sarek_fatal("data shortage", EXIT_DATA_SHORTAGE);

	if (!(row = mysql_fetch_row(sarek_result))) {
		free_result();
		response_eod();
		return;
	}

	colval(row[0]);

	for(i = 1; i < sarek_ncol; i++) {
		putchar(sarek_sep);
		colval(row[i]);
	}

	response_end();
}

void free_result(void) {
	if (sarek_debug)
	fprintf(stderr, "FREE RESULT\n");

	if (!sarek_result)
	sarek_fatal("unexpected free result directive", EXIT_FREE_RESULT);

	mysql_free_result(sarek_result);
	sarek_result = NULL;
}

static void colval(char *s) {
	if (!s) {
		fputs(sarek_null, stdout);
		return;
	}

	for (; *s; s++) {
		switch (*s) {
		case '\n':
			putchar('\\');
			putchar('n');
			break;
		case '\t':
			putchar('\\');
			putchar('t');
			break;
		default:
			putchar(*s);
			break;
		}
	}
}
