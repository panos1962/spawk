#include "sarek.h"

void response_begin(const int code) {
	printf("%d%c", code, sarek_sep);
}

void response_end(void) {
	putchar('\n');
	fflush(stdout);
}

void response_eod(void) {
	putchar(sarek_eod);
	putchar('\n');
	fflush(stdout);
}
