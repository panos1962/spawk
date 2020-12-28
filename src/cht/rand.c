/*
**	@(#) cht_lrand, cht_drand 26/10/2000
**
**	Copyright (C) 2000 Panos I. Papadopoulos
**		All Rights Reserved.
**
** Οι functions cht_lrand και cht_drand επιστρέφουν τυχαίους
** αριθμούς στο διάστημα που καθορίζουν οι παράμετροι min και
** max.  Πιο συγκεκριμένα, οι αριθμοί που επιστρέφονται είναι
** στο διάστημα [min, max).  Η cht_lrand επιστρέφει long αριθμούς,
** ενώ η cht_drand επιστρέφει double.
**
** Η παραγωγή των τυχαίων αριθμών βασίζεται στη function drand48,
** ενώ η αρχικοποίηση της γεννήτριας γίνεται μέσω της function
** srand48 με παράμετρο έναν long που προκύπτει από την τρέχουσα
** ώρα συν το process-id.
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <cht.h>

static char srand_ = 1;

static void rnd_set(void)
{
	long t;
	long p;

	p = (long)getpid();
	if (time(&t) < 0L) {
		cht_error(NULL);
		perror("cht_rand: time");
		t = p;
	}
	else
		t += p;

	srand48(t);
	srand_ = 0;
}

long cht_lrand(long min, long max)
{
	if (min > max) {
		cht_error(NULL);
		fprintf(stderr, "[%ld, %ld): invalid lrand range\n",
			min, max);
		return min;
	}

	if (srand_)
		rnd_set();

	return (long)(min + (drand48() * (max - min)));
}

double cht_drand(double min, double max)
{
	if (min > max) {
		cht_error(NULL);
		fprintf(stderr, "[%lf, %lf): invalid lrand range\n",
			min, max);
		return min;
	}

	if (srand_)
		rnd_set();

	return min + (drand48() * (max - min));
}
