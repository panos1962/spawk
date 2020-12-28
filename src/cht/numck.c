#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <cht.h>

#define SNULL ((char *)(NULL))

int cht_numck(char *s, long min, long max)
{
	long l;
	register char *p = s;
	double d;
	int no_digit = 1;

	if (s == (char *)0) {
		if (cht_valck_errmsg != SNULL) {
			cht_error(SNULL);
			fprintf(stderr, "(nil): %s\n",
				cht_valck_errmsg);
		}
		else
			cht_error("numck: null string");
		return 1;
	}

	switch (*p) {
	case '+':
	case '-':
		p++;
		break;
	}

	while (*p) {
		if (!isdigit(*p)) {
			cht_error(NULL);
			if (cht_valck_errmsg != SNULL)
				fprintf(stderr, "%s: %s\n", s,
					cht_valck_errmsg);
			else {
				char c;

				fprintf(stderr, "%s", s);
				c = *p;
				*p = 0;
				fprintf(stderr, ": %s", s);
				*p = c;
				fprintf(stderr, "^%s: no digit\n", p);
			}

			return CHT_NUMCK_DIGIT;
		}

		p++;
		no_digit = 0;
	}

	if (no_digit || ((d = atof(s)) < min) || (d > max) ||
		((l = atol(s)) < min) || (l > max)) {
		cht_error(NULL);
		if (cht_valck_errmsg)
			fprintf(stderr, "%s: %s\n", s,
				cht_valck_errmsg);
		else
			fprintf(stderr, "%s: out of range "
				"[%ld, %ld]\n", s, min, max);
		return CHT_NUMCK_RANGE;
	}

	return 0;
}

int cht_ulnumck(char *s, unsigned long min, unsigned long max)
{
	unsigned long l;
	register char *p = s;
	int no_digit = 1;

	if (s == (char *)0) {
		cht_error("ulnumck: null string");
		return 1;
	}

	switch (*p) {
	case '+':
		p++;
		break;
	}

	while (*p) {
		if (!isdigit(*p)) {
			char c;

			cht_error(NULL);
			fprintf(stderr, "%s", s);
			c = *p;
			*p = 0;
			fprintf(stderr, ": %s", s);
			*p = c;
			fprintf(stderr, "^%s: no digit\n", p);
			return CHT_NUMCK_DIGIT;
		}

		p++;
		no_digit = 0;
	}

	if (no_digit || (atof(s) > (unsigned long)(~0L)) ||
		((l = strtoul(s, (char **)NULL, 0)) < min) ||
		(l > max)) {
		cht_error(NULL);
		fprintf(stderr, "%s: out of range [%lu, %lu]\n",
			s, min, max);
		return CHT_NUMCK_RANGE;
	}

	return 0;
}
