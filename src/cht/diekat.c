/*
** Η function cht_diekat δέχεται παραμέτρους την οδό,
** τον αριθμό, την πόλη, τη χώρα και τον ταχυδρομικό κώδικα
** και επιστρέφει τη διεύθυνση κατοικίας.
** Το string που κατασκευάζεται τοποθετείται στην πρώτη
** παράμετρο αν αυτή δεν είναι είναι null.
** Αν η πρώτη παράμετρος είναι null, τότε το string αποθηκεύεται
** σε εσωτερική static μεταβλητή, η οποία επαναγράφεται κάθε φορά
** που καλείται η συνάρτηση.
*/

#include <stdlib.h>
#include <stdio.h>
#include <cht.h>

char *cht_diekat(char *s, char *odos, char *odar,
	char *poli, char *xora, char *tk)
{
	static char dk[256];
	char *p;
	char *target;

	if (s == (char *)0) {
		target = dk;
		p = dk;
	}
	else {
		target = s;
		p = s;
	}

	if (odos != (char *)0) {
		while (*odos == ' ')
			odos++;
		if (*odos)
			p = cht_strcpy(p, odos);
	}

	if (odar != (char *)0) {
		while (*odar == ' ')
			odar++;
		if (*odar) {
			if (p > target)
				*p++ = ' ';
			p = cht_strcpy(p, odar);
		}
	}

	if (poli != (char *)0) {
		while (*poli == ' ')
			poli++;
		if (*poli) {
			if (p > target)
				*p++ = ' ';
			p = cht_strcpy(p, poli);
		}
	}

	if (xora != (char *)0) {
		while (*xora == ' ')
			xora++;
		if (*xora) {
			if (p > target)
				*p++ = ' ';
			p = cht_strcpy(p, xora);
		}
	}

	if (tk != (char *)0) {
		while (*tk == ' ')
			tk++;
		if (*tk) {
			if (p > target)
				*p++ = ' ';
			p = cht_strcpy(p, tk);
		}
	}

	*p = 0;
	return target;
}
