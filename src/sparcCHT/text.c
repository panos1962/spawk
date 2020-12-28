#include "sparc.h"

/*
** Η function `txt_alloc' δέχεται ως παράμετρο ένα text segment
** (όχι απαραίτητα ολοκληρωμένη γραμμή) και δημιουργεί ένα
** TEXT node με το εν λόγω segment σώζοντας το string κάπου στη
** μνήμη και επιστρέφει τη διεύθυνση του νέου node.
*/

static TEXT *txt_alloc(char *s, int len)
{
	TEXT *t;

	if (((void *)(t = (TEXT *)malloc(sizeof(TEXT)))) == NULL)
		cht_nomem("txt_alloc", EXIT_MEMORY);

	t->text = cht_strnsave(s, len, EXIT_MEMORY);
	t->next = TXT_NULL;
	t->last = TXT_NULL;
	return(t);
}

/*
** Η function `txt_add' προσθέτει το text segment που της περνάμε ως
** δεύτερη παράμετρο στο text που της περνάμε ως πρώτη παράμετρο.
** Το data segment δεν είναι απαραίτητα ολοκληρωμένη γραμμή.
** Η function επιστρέφει πάλι την αρχή του text δηλαδή είτε τον
** text pointer που της περάσαμε ως πρώτη παράμετρο εφόσον αυτός
** δεν ήταν null, είτε το νέο text node που θα είναι και η αρχή
** του text εφόσον της περάσαμε null text pointer.
*/

TEXT *txt_add(TEXT *t, char *s, int len)
{
	TEXT *q;

	if (s == STR_NULL)
		cht_fatal("txt_add: null line (internal error)",
			EXIT_FATAL);

	/*
	** Δεν έχει νόημα να κρατήσω null data segments οπότε
	** αν το string που πέρασα είναι κενό, το απορρίπτω.
	*/

	if (len <= 0)
		return(t);

	q = txt_alloc(s, len);
	if (t == TXT_NULL) {
		q->last = q;
		return(q);
	}

	t->last->next = q;
	t->last = q;
	return(t);
}

/*
** Η function `txt_more' προσθέτει (append) το κείμενο που της περνάμε
** ως δεύτερη παράμετρο στο κείμενο που της περνάμε ως πρώτη παράμετρο
** και επιστρέφει την αρχή του κειμένου.
*/

TEXT *txt_more(TEXT *t1, TEXT *t2)
{
	if (t1 == TXT_NULL)
		return(t2);

	if (t2 == TXT_NULL)
		return(t1);

	t1->last->next = t2;
	t1->last = t2->last;
	return(t1);
}

/*
** Η function `txt_print' τυπώνει το κείμενο που της περνάμε ως πρώτη
** παράμετρο στον file pointer που της περνάμε ως δεύτερη παράμετρο
** παρεμβάλλοντας tabs στην αρχή κάθε γραμμής. Ως τρίτη παράμετρο
** περνάμε το πλήθος των tabs που επιθυμούμε να τυπώνουμε. Συνήθως
** το κείμενο εκτυπώνεται στο standard ouput (stdout), αλλά στο
** debuging είναι χρήσιμο να τυπώσω και σε άλλους file pointers (π.χ.
** stderr). Τα tabs χρειάζονται για ομορφότερη εκτύπωση του παραγόμενου
** awk κώδικα, οπότε ανάλογα με το κεφάλαιο τυπώνω και τα ανάλογα tabs.
*/

void txt_print(TEXT *t, FILE *fp, int tabs)
{
	TEXT *q;
	char *p;
	int i;
	int nl = 1;

	for (q = t; q != TXT_NULL; q = q->next) {
		if (nl) {
			for (i = 0; i < tabs; i++)
				putchar('\t');

			nl = 0;
		}

		for (p = q->text; *p; p++) {
			if (*p == '\n')
				nl = 1;

			putc(*p, fp);
		}
	}
}
