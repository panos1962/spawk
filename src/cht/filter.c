/*
** Η function cht_filter διοχετεύει το standard output
** του προγράμματος σε pipe που ανοίγει προς το
** πρόγραμμα που της περνάμε σαν πρώτη παράμετρο.
** Θα αναφέρω κάποιο παράδειγμα με κάποιον report processor
** έστω ονομαζόμενο RPT.
** Εστω λοιπόν, ότι ο RPT διατίθεται στο εμπόριο μαζί με τις
** βιβλιοθήκες του (object archives) και με κάποιες τεχνικές
** εμπλουτισμού με user defined C-functions (C-hook functions).
** Μπορούμε να εκτελέσουμε στο `before report' section του
** RPT script την εντολή init_report, η οποία θα καλεί την
** cht_filter("nroff") και έτσι, ό,τι τυπώσουμε με τον RPT
** θα περάσει από τον nroff για περαιτέρω επεξεργασία.
*/

#include <stdlib.h>
#include <stdio.h>
#include <cht.h>

static void filter_wait(void)
{
	fflush(stdout);
	fclose(stdout);
	while (wait((int *)0) != -1)
		;
}

void cht_filter(char *prog, int err)
{
	FILE *fp;
	static int first_time = 1;

	if ((void *)(fp = popen(prog, "w")) == NULL)
		goto ERROR;

	if (close(1) == -1)		/* close stdout */
		goto ERROR;

	if (dup2(fileno(fp), 1) == -1)	/* make stdout equals fp */
		goto ERROR;

	if (close(fileno(fp)) == -1)	/* pipe is now stdout */
		goto ERROR;

	if (first_time) {
		atexit(filter_wait);
		first_time = 0;
	}

	return;

ERROR:
	cht_error(NULL);
	fputs("cht_filter: ", stderr);
	perror(prog);
	exit(err);
}
