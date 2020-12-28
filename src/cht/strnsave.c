/*
** Η function cht_strnsave "σώζει" στη μνήμη το string
** που της περνάμε σαν παράμετρο και επιστρέφει την διεύθυνση
** μνήμης που έσωσε το string.
** Η function θεωρεί ότι το string έχει l χαρακτήρες και έτσι
** παίρνει χώρο μνήμης με l + 1 χαρακτήρες, διότι τοποθετεί
** στο τέλος του string μηδενικό χαρακτήρα.
** Αν η μνήμη δεν επαρκεί, τότε τυπώνεται μήνυμα λάθους
** και γίνεται exit με exit status err.
*/

#include <memory.h>
#include <cht.h>

char *cht_strnsave(char *s, int l, int err)
{
	char *p;

	if ((void *)(p = malloc(l + 1)) == NULL)
		cht_fatal("strnsave: out of memory", err);

	memcpy(p, s, l);
	*(p + l) = 0;
	return p;
}
