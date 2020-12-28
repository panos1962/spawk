#include <stdlib.h>
#include <stdio.h>
#include <cht.h>

short cht_device(void)
{
	char *p;

	if ((p = getenv("CHT_DEVICE")) == (char *)0)
		return 0;

	return atoi(p);
}
