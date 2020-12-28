#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <cht.h>

/*
** Η function `cht_triad' δέχεται ένα double και ετοιμάζει ένα
** string με τον αριθμό αυτό τυπωμένο κατά χιλιάδες (triads).
** Σεν πρώτη παράμετρο περνάμε έναν character pointer στον
** οποίο θα γραφεί το αποτέλεσμα· αν αυτός ο pointer είναι
** null τότε το αποτέλεσμα ετοιμάζεται σε εσωτερικό static
** string και επιστρέφεται η διεύθυνση αυτού του string,
** αλλιώς επιστρέφεται η διεύθυνση του string που περάσαμε.
**
** Η παράμετρος `no_miden' δείχνει αν θα τυπωθεί ψηφίο `0' σε
** περίπτωση που το νούμερο είναι μηδέν.  Αν η παράμετρος
** `no_miden' είναι μηδέν τότε τυπώνεται ψηφίο (0) αλλιώς
** δεν τυπώνεται τίποτα.
**
** Η παράμετρος `deka' δείχνει το μέγεθος του δεκαδικού μέρους.
** Φυσικά το αποτέλεσμα στρογγυλεύεται στο μήκος αυτό.
*/

#define MAX_LEN 63

char *cht_triad(char *s, double x, int no_miden, int deka)
{
	static char buf[MAX_LEN + 1];
	char t[MAX_LEN + 1];
	char *first_digit;
	register char *p;
	register char *q;
	register int n;

	/*
	** Ο pointer `q' είναι αυτός που διατρέχει το string
	** που κατασκευάζουμε.  Αρχικά λοιπόν δείχνει το `s'
	** το οποίο μπορεί να δείχνει και στο εωτερικό buffer.
	*/

	if (s == (char *)0)
		s = buf;
	q = s;

	if (deka < 0) {
		cht_error(NULL);
		fprintf(stderr, "cht_triad: %d: invalid decimal "
			"part length\n", deka);
		deka = 0;
	}

	sprintf(t, "%.*lf", deka, x);
	if (*t == '-') {
		*q++ = '-';
		first_digit = t + 1;
	}
	else
		first_digit = t;

	/*
	** Αν δεν θέλω εκτύπωση ψηφίου (0) για μηδενικά νούμερα
	** επιστρέφω κενό string.
	*/

	if (no_miden) {
		for (p = first_digit; *p; p++) {
			if (isdigit(*p) && (*p != '0'))
				goto NON_ZERO;
		}
		*s = 0;
		return s;
	}

NON_ZERO:
	/*
	** Θα μετρήσουμε τώρα το μήκος του ακέραιου μέρους.
	*/

	for (p = first_digit; *p; p++) {
		if (*p == '.')
			break;
	}

	/*
	** Τώρα, ο `p' έχει σταματήσει στο τέλος του ακέραιου
	** μέρους.  Ο pointer `first_digit' δείχνει το πρώτο
	** ψηφίο, επομένως (p - first_digit) είναι το μήκος
	** του ακέραιου μέρους του αριθμού το οποίο θα σπάσει
	** σε τριάδες.  Η μεταβλητή `n' δείχνει πόσα ψηφία
	** της τριάδας έχω ήδη τυπώσει.
	*/

	if ((n = 3 - ((p - first_digit) % 3)) >= 3)
		n = 0;

	/*
	** Γράφω το ακέραιο μέρος του αριθμού παρεμβάλλοντας
	** διαχωριστικό (κόμα) όπου χρειάζεται.
	*/

	for (p = first_digit; *p; p++) {
		if (*p == '.')
			break;

		if (n++ == 3) {
			*q++ = ',';
			n = 1;
		}
		*q++ = *p;
	}

	/*
	** Γράφω τώρα και το δεκαδικό μέρος του αριθμού.
	** Εδώ θα τερματιστεί και το string.
	*/

	while (*p)
		*q++ = *p++;
	*q = 0;
	return s;
}
