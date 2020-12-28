#include <cht.h>
#include <ctype.h>

long cht_round(double x)
{
	int monada = 1;
	char buf[128];
	char *p;
	long n;

	sprintf(buf, "%.10lf", x);
	if (*buf == '-') {
		p = buf + 1;
		monada = -1;
	}
	else
		p = buf;

	for (; *p; p++) {
		if (*p == '.')
			break;

	}

	*p++ = 0;
	if (!(*p))
		goto ERROR;

	for (; *p; *p++) {
		if (!isdigit(*p))
			goto ERROR;

		if (*p <= '4')
			return(atol(buf));

		if (*p > '4')
			return(atol(buf) + monada);
	}

	return(atol(buf));

ERROR:
	cht_error(NULL);
	fprintf(stderr, "cht_round: %s: invalid float\n", buf);
	sprintf(buf, "%.0lf", x);
	return(atol(buf));
}
