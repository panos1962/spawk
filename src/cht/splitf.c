/*
** Η function cht_splitf σπάζει κάποιο string σε πεδία
** που χωρίζονται μεταξύ τους με κάποιον χαρακτήρα διαχωρισμού.
** Η function καλείται επαναληπτικά για κάθε πεδίο και την
** πρώτη φορά περνάμε σαν παράμετρο το record, ενώ τις
** υπόλοιπες περνάμε έναν null pointer.
** Η function επιστρέφει την αρχή του field μέσα στο record
** ή null αν δεν υπάρχουν άλλα fields.
** Η function αλλοιώνει το αρχικό string, τοποθετώντας
** μηδενικό χαρακτήρα στο τέλος κάθε πεδίου.
*/

#include <stdlib.h>
#include <stdio.h>
#include <cht.h>

char *cht_splitf(char *s, char sep)
{
	static char *p0 = (char *)0;
	register char *p;

	if (s != (char *)0)
		p0 = s;
	else if (p0 == (char *)0)
		return (char *)0;

	for (p = s = p0; *p; p++) {
		if (*p == sep) {
			*p = '\0';
			p0 = p + 1;
			return s;
		}
	}

	p0 = (char *)0;
	return s;
}
