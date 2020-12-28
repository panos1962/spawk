#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <cht.h>

/*
** Η function dtol μετατρέπει τον double που δέχεται σαν παράμετρο
** στον κοντινώτερο long, τον οποίο και επιστρέφει.
** Το μισό "στρογγυλεύεται" στον ακέραιο που βρίσκεται πιο
** μακρυά από το μηδέν π.χ. 4.5 --> 5, -4.5 --> -5
*/

long cht_dtol(double d)
{
	long n = (long)d;	/* truncate fractional part */

	if (d > 0)
		return ((d - n) < 0.5 ? (n) : (n + 1));
	else
		return ((n - d) >= 0.5 ? (n - 1) : (n));
}
