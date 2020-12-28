/*
** Η function cht_range είναι πολύ απλή αλλά πολύ χρήσιμη function.
** Δέχεται σαν παράμετρο ένα double και δύο όρια και ελέγχει αν
** η τιμή της παραμέτρου βρίσκεται εντός των ορίων (inclusive).
** Αν η τιμή είναι εντός ορίων επιστρέφει true αλλοιώς επιστρέφει
** false.
*/

#include <stdlib.h>
#include <stdio.h>
#include <cht.h>

int cht_range(double x, double min, double max)
{
	if ((x > max) || (x < min))
		return 0;

	return 1;
}
