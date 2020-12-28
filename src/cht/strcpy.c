/*
** Μία "έξυπνη" strcpy.
** Επιστρέφει το τέλος του string αντί για την αρχή του.
** Ετσι, μπορεί να κληθεί επαναληπτικά για να κατασκευάσουμε
** ένα string από συνένωση πολλών strings.
**/

#include <stdlib.h>
#include <stdio.h>
#include <cht.h>

char *cht_strcpy(char *dst, char *src)
{
	while (*src)
		*dst++ = *src++;
	*dst = 0;

	return dst;
}
