#include <ctype.h>
#include <cht.h>

/*
** Η function cht_error δέχεται σαν παράμετρο ένα string
** και τυπώνει μήνυμα λάθους στο standard error.
** Το μήνυμα περιλαμβάνει το όνομα προγράμματος,
** το οποίο βρίσκεται στην global μεταβλητή cht_progname
** και τίθεται συνήθως από την function cht_set_progname.
** Αν το μήνυμα είναι ένας null pointer, τότε τυπώνεται
** απλά το όνομα του προγράμματος και έτσι μπορούμε να
** τυπώσουμε αργότερα ό,τι άλλο θέλουμε π.χ. με την fprintf κλπ.
** Η cht_fatal καλεί την cht_error και κατόπιν κάνει exit
** με την τιμή που περνάμε σαν δεύτερη παράμετρο.
** Η cht_nomem είναι ειδική περίπτωση της cht_error
** για λάθη που οφείλονται σε έλλειψη μνήμης,
** τα οποία εμφανίζονται αρκετά συχνά στα προγράμματα.
*/

void cht_error(char *s)
{
	if ((void *)cht_progname != NULL) {
		fputs(cht_progname, stderr);
		fputs(": ", stderr);
	}

	if ((void *)s != NULL) {
		fputs(s, stderr);
		putc('\n', stderr);
	}
}

void cht_fatal(char *s, int err)
{
	if (s == NULL)
		s = "fatal error";

	cht_error(s);
	exit(err);
}

int cht_erropt(void)
{
	extern int optopt;

	cht_error(NULL);
	fprintf(stderr, "illegal option -- %c\n",
		(isprint(optopt) ? optopt : '?'));
	return 1;
}

void cht_nomem(char *msg, int err)
{
	cht_error(NULL);
	if (msg != NULL) {
		fputs(msg, stderr);
		fputs(": ", stderr);
	}

	fputs("out of memory\n", stderr);
	exit(err);
}
