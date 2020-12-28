#include <cht.h>

char *cht_fltprint(char *s, double x)
{
	static char buf[64];
	register char *p;
	register char *q;

	if (s == (char *)NULL)
		s = buf;

	sprintf(s, "%.6lf", x);
	for (p = s; *p; p++) {
		if (*p == '.')
			break;
	}

	if (*p != '.')
		return s;

	for (q = p + 1; *q; q++) {
		if (*q != '0')
			p = q + 1;
	}

	*p = 0;
	return s;
}
