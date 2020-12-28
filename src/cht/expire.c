/*
** Η function cht_expire καλείται συνήθως στην αρχή του
** προγράμματος (main function) και μετά την cht_set_progname.
** Δέχεται παραμέτρους τον μήνα, τη μέρα και το έτος
** που επιθυμούμε να λήξει η άδεια χρήσης του προγράμματος,
** καθώς επίσης και ένα exit status, το οποίο θα χρησιμοποιηθεί
** στο exit.
*/

#include <stdlib.h>
#include <stdio.h>
#include <cht.h>

void cht_expire(int m_exp, int d_exp, int y_exp, int err)
{
	int mdyhms[6];
	int d;
	int m;
	int y;
	int dy;
	int dm;
	int dd;
	int days_left;

	if ((y_exp < 0) || (m_exp < 0) || (m_exp > 12) ||\
		(d_exp < 0) || (d_exp > 31))
		cht_fatal("invalid expiration date", err);

	cht_curtime(mdyhms);

	if (mdyhms[2] > y_exp)
		goto EXPIRED;

	if (mdyhms[2] < y_exp)
		goto CHECK;

	if (mdyhms[0] > m_exp)
		goto EXPIRED;

	if (mdyhms[0] < m_exp)
		goto CHECK;

	if (mdyhms[1] > d_exp)
		goto EXPIRED;

	if (mdyhms[1] < d_exp)
		goto CHECK;

EXPIRED:
	cht_fatal("program expired", err);

CHECK:
	d = d_exp;
	m = m_exp;
	y = y_exp;

	while ((dd = d - mdyhms[1]) < 0) {
		d += 31;
		m--;
	}
	while ((dm = m - mdyhms[0]) < 0) {
		m += 12;
		y--;
	}
	dy = y - mdyhms[2];

	if ((days_left = (dy * 12 * 31) + (dm * 31) + dd) < 30) {
		cht_error((char *)0);
		fprintf(stderr, "program will expire soon "
			"(%d/%d/%d)\n", d_exp, m_exp, y_exp);
	}
}
