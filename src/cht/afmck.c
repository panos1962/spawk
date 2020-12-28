#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <cht.h>

/*
** Η function safmck ελέγχει το parity digit (τελευταίο ψηφίο)
** ενός Α.Φ.Μ. σύμφωνα με αλγόριθμο που βρέθηκε στο internet από
** τη Δώρα Καργάκη (kargador) το 2004.
**
** Η function επιστρέφει μηδέν αν το string afm που της περνάμε
** ως μια και μοναδική παράμετρο είναι έγκυρο Α.Φ.Μ., αλλιώς
** επιστρέφει: ένα θετικό όταν κάποιος από τους χαρακτήρες του
** string δεν είναι ψηφίο (επιστρέφει τη θέση του απαράδεκτου
** χαρακτήρα 1-based), και ένα αρνητικό αριθμό που δείχνει το
** είδος του λάθους όταν το string δεν είναι έγκυρο Α.Φ.Μ.
** Πιο συγκεκριμένα, επιστρέφει (-1) όταν το Α.Φ.Μ. είναι μικρό,
**
** Ο αλγόριθμος
** ------------
** Αν ο Α.Φ.Μ. είναι A1A2A3A4A5A6A7A8A9, όπου Ai είναι το i ψηφίο
** του αριθμού, τότε υπολογίζω το άθροισμα:
**
**	tot = (256 * A1) + (128 * A2) + (64 * A3) + (32 * A4) +
**		(16 * A5) + (8 * A6) + (4 * A7) + (2 * A8)
**
** Υπολογίζω το υπόλοιπο Y της διαίρεσης του tot με το 11.
** Αν το Y είναι 10 τότε A9 = 0, αλλιώς A9 = Y.
**
** Μη χρησιμοποιείται το macro isdigit για τον έλεγχο του
** χαρακτήρα, γιατί μπορεί να μη δουλεύει καλά με τα ελληνικά.
** Π.χ. στο AViiON το isdigit για το ελληνικό κεφαλαίο ζήτα
** (Ζ) επιστρέφει TRUE (1).
*/

int cht_safmck(char *afm)
{
	register char *p;
	register short base;
	register long tot;
	int parity;

	for (tot = 0L, base = 256, p = afm; base > 1; p++, base /= 2) {
		if (*p == 0)
			return (-1);

		if (((*p) < '0') || ((*p) > '9'))
			return (p - afm + 1);

		tot += base * ((*p) - '0');
	}

	if (*p == 0)
		return (-1);

	if ((*(p + 1)) != 0)
		return (-2);

	if (((*p) < '0') || ((*p) > '9'))
		return 9;

	if ((parity = tot % 11L) == 10)
		parity = 0;

	if (((*p) - '0') != parity)
		return (-3);
	else
		return (0);
}

/*
** Η function nafmck δέχεται ως μια και μοναδική παράμετρο ένα
** long και ελέγχει αν είναι δεκτό Α.Φ.Μ.
*/

int cht_nafmck(long afm)
{
	char buf[32];

	sprintf(buf, "%09ld", afm);
	return (cht_safmck(buf));
}

/*
** Η fucntion fixafm δέχεται ως παράμετρο ένα string και, αφού
** αγνοήσει τα κενά στην αρχή και στο τέλος, επιστρέφει το Α.Φ.Μ.
** ως long. Αν επιστρέψει 0 τότε υπάρχει πρόβλημα.
*/

long cht_fixafm(char *s)
{
	register char *p;
	register char *q;
	long afm;

	if (s == (char *)NULL)
		return 0L;

	for (p = s; *p; p++) {
		if (!isspace(*p))
			break;
	}

	if (!(*p))
		return 0L;

	for (q = p; *q; q++) {
		if (!isdigit(*q))
			break;
	}

	if (q <= p)
		return 0L;

	if (q - p > 9)
		return 0L;

	for (; *q; q++) {
		if (!isspace(*q))
			return 0L;
	}

	return (cht_nafmck(afm = atol(p)) ? 0L : afm);
}
