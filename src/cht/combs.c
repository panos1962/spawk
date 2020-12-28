/*
** Η function combs υπολογίζει το πλήθος των συνδυασμών
** των n πραγμάτων ανά m.
** Οι πράξεις γίνονται σε double precission και το αποτέλεσμα
** επιστρέφεται σε long.  Αυτό γίνεται για να έχουμε μεγαλύτερο εύρος.
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cht.h>

long cht_combs(register int n, register int m)
{
	register double n1 = 1.0;

	if ((m <= 0) || (n < m) || (n <= 0))
		return 0L;

	while (m > 0)
		n1 *= ((double)(n--)) / (m--);
	return (n1 - floor(n1) < 0.5 ? (long)n1 : (long)n1 + 1);
}
