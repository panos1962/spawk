#include "sparc.h"
#include <string.h>

void set_progname(char *s)
{
	int l;
	char *p;

	if (remove_script)
		return;

	if ((l = strlen(s)) <= 0)
		return;

	if ((p = (char *)malloc(l + 32)) == STR_NULL)
		return;

	sprintf(p, "cht_progname=%s", s);
	putenv(p);
}
