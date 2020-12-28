#include <cht.h>

/*
** Η fucntion cht_strunc κόβει τα κενά από το τέλος
** του string που της περνάμε σαν παράμετρο
** και επιστρέφει την αρχή του string.
*/

char *cht_strunc(char *s)
{
	register char *p;
	register char *q;

	if ((void *)s == NULL)
		return s;

	for (p = s, q = s - 1; *p; p++) {
		if (*p != ' ')
			q = p;
	}

	*(++q) = 0;
	return s;
}
