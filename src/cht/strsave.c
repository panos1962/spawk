/*
** Η function cht_strsave "σώζει" στη μνήμη το string
** που της περνάμε σαν παράμετρο και επιστρέφει την διεύθυνση
** μνήμης που έσωσε το string.
** Αν η μνήμη δεν επαρκεί, τότε τυπώνεται μήνυμα λάθους
** και γίνεται exit με exit status err.
*/

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <cht.h>

char *cht_strsave(char *s, int err)
{
	char *p;
	int l;

	if (s == (char *)0)
		return s;

	l = strlen(s) + 1;
	if ((void *)(p = malloc(l)) == NULL)
		cht_fatal("strsave: out of memory", err);

	memcpy(p, s, l);
	return p;
}
