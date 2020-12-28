/*
** Επιλύει σύστημα n κανονικών εξισώσεων με n αγνώστους.
** Δέχεται παραμέτρους:
**
** double *a;
** ---------
** Είναι ένας μονοδιάστατος πίνακας με
** a11, a12, a13,..., a1n, c1, a21,..., a2n, c2,...
** το ένα μετά το άλλο.
** Δηλαδή οι συντελεστές των εξισώσεων κατά γραμμή
** (και ο σταθερός όρος)
**
** int n;
** ------
** Το πλήθος των εξισώσεων.
**
** double *x;
** ----------
** Ο πίνακας όπου θα μπούν οι λύσεις του συστήματος.
*/

#include <stdlib.h>
#include <stdio.h>
#include <values.h>
#include <math.h>
#include <cht.h>

static int find(double *a, int n, register int i)
{
	register double *p;
	int nn = n + 1;

	p = a + i * nn + i;

	for (; i < n; i++, p += nn) {
		if (fabs(*p) >= MINDOUBLE)
			return i;
	}

	return -1;
}

static void change(double *a, int n, register int i, int j)
{
	register double *pi;
	register double *pj;
	int nn = n + 1;
	double t;

	for (pi = a + i * nn + i, pj = a + j * nn + i;
		i <= n; i++, pi++, pj++) {
		t = *pi;
		*pi = *pj;
		*pj = t;
	}
}

static void rowmul(double *a, int n, int i, int j)
{
	register double *pi;
	register double *pj;
	double d;
	int nn = n + 1;
	register int k;

	pi = a + i * nn + i;
	pj = a + j * nn + i;
	d = (*pj) / (*pi);
	*pj = 0.0;
	for (pi++, pj++, k = i + 1; k < nn; k++, pi++, pj++)
		*pj -= (*pi) * d;
}

int cht_gauss(double *a, int n, double *x)
{
	int nn = n - 1;
	register int i;
	int j;
	register int ii;
	register double *p;
	register double *q;
	double *p1;
	double s;

	for (i = 0; i < nn; i++) {
		if ((j = find(a, n, i)) == -1)
			return 1;
		change(a, n, i, j);
		for (ii = i + 1; ii < n; ii++)
			rowmul(a, n, i, ii);
	}

	nn = n + 1;

	for (ii = n - 1; ii >= 0; ii--) {
		p = a + ii * nn + n;
		p1 = a + ii * nn + ii;
		if (fabs(*p1) < MINDOUBLE)
			return 1;
		q = x + n - 1;
		for (s = *(p--); p > p1; p--, q--)
			s += (*p) * (*q);
		*q = -s / *p1;
	}

	return 0;
}
