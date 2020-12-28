#include <cht.h>

char *cht_cfill(char *s, char c, int n)
{
	if (s == ((char *)(NULL)))
		return((char *)(NULL));

	if (n <= 0)
		return(s);

	while (n-- > 0)
		*s++ = c;

	return(s);
}
