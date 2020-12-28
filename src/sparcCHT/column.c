#include "sparc.h"
#include <string.h>

/*
** Η function `col_alloc' δημιουργεί έναν DB_COLUMN κόμβο για τον
** πίνακα που της περνάμε ως πρώτη παράμετρο (DB_TABLE) και τη στήλη
** που της περνάμε ως δεύτερη παράμετρο (string). Ως τρίτη παράμετρο
** περνάμε το μήκος της στήλης καθώς αυτό είναι γνωστό στο σημείο που
** καλείται η function (lexical analyzer), ώστε να κληθεί η
** `cht_strnsave' αντί της `cht_strsave' που θα έπρεπε εκ νέου να
** μετρήσει το μήκος του string.
*/

DB_COLUMN *col_alloc(DB_TABLE *table, char *column, int len)
{
	DB_COLUMN *p;

	if (len <= 0)
		cht_fatal("col_alloc: invalid column name length "
			"(internal error)", EXIT_FATAL);

	if (((void *)(p = (DB_COLUMN *)
		malloc(sizeof(DB_COLUMN)))) == NULL)
		cht_nomem("col_alloc", EXIT_MEMORY);

	p->table = table;
	p->column = cht_strnsave(column, len, EXIT_MEMORY);
	p->next = col_list;
	return(col_list = p);
}

/*
** Η function `tbl_add' προσθέτει στο `tbl_list' έναν νέο πίνακα.
** Η `tbl_list' είναι linked list που περιέχει όλους τους πίνακες
** της database που εμφανίζονται είτε μέσα στα ονόματα μεταβλητών
** είτε ως data types. Η λίστα περιέχει τους που εμφανίστηκαν κατά
** καιρούς μια μόνο φορά τον καθένα ασχέτως με το πόσες φορές
** εμφανίζονται στο awkrpt script, επομένως πρέπει να την καλώ αφού
** πρώτα ελέγξω αν ο προς εισαγωγή πίνακας είναι ήδη ενταγμένος.
** Επειδή στο σημείο που καλείται η function (lexical analyzer) είναι
** γνωστό και το μήκος του ονόματος του πίνακα, το περνώ ως δεύτερη
** παράμετρο ώστε να χρησιμοποιήσω την `cht_strnsave' αντί της
** `cht_strsave' η οποία θα έπρεπε να μετρήσει το μήκος του string.
** Η function ενημερώνει τη λίστα `tbl_list' βάζοντας ως πρώτο
** στοιχείο τον νέο κόμβο, επομένως η εν λόγω λίστα είναι LIFO.
*/

static DB_TABLE *tbl_add(char *table, int len)
{
	DB_TABLE *t;

	if (len <= 0)
		cht_fatal("tbl_add: invalid table name length "
			"(internal error)", EXIT_FATAL);

	if (((void *)(t = (DB_TABLE *)
		malloc(sizeof(DB_TABLE)))) == NULL)
		cht_nomem("tbl_add", EXIT_MEMORY);

	t->name = cht_strnsave(table, len, EXIT_MEMORY);
	t->next = tbl_list;
	return(tbl_list = t);
}

/*
** Η function `col_add' καλείται κάθε φορά που ο lexical analyzer
** αναγνωρίζει database column. Τα database columns χρησιμοποιούνται
** στα awkrpt scripts είτε ως μεταβλητές είτε ως data type specifiers.
** Όλα τα database columns που αναγνωρίζονται εντάσσονται (μοναδικά)
** στη linked list `col_list'. Παρεμπιπτόντως εντάσσονται και οι
** πίνακες στη λίστα των πινάκων `tbl_list'. Η function ελέγχει τη
** λίστα `col_list' πριν εντάξει το database column και αν το εντοπίσει
** επιστρέφει τη διεύθυνση του υπάρχοντα κόμβου, αλλιώς προσθέτει νέο
** κόμβο, τον τοποθετεί πρώτο στη λίστα και επιστρέφει τη διεύθυνση
** του νέου κόμβου. Στη function περνάμε το database column ως string,
** π.χ. prosop_person.pekey, και το μήκος του string εφόσον αυτό είναι
** γνωστό στο σημείο που καλείται η function (lexical analyzer).
*/

DB_COLUMN *col_add(char *s, int len)
{
	register char *p;
	int tlen;		/* table's name length */
	int clen;		/* column's name length */
	DB_COLUMN *t;
	DB_TABLE *table;

	if (s == STR_NULL)
		cht_fatal("col_add: null database column name "
			"(internal error)",
			EXIT_FATAL);

	for (p = s; *p; p++) {
		if (*p == '.')
			break;
	}

	if (((tlen = p - s) <= 0) || ((clen = len - tlen - 1) <= 0)) {
		yyerror(NULL);
		fprintf(stderr, "col_add: %s: "
			"invalid database column name\n", s);
	}

	/*
	** Ο DB_TABLE pointer `table' τίθεται αρχικά null και
	** κατόπιν ελέγχεται ώστε να δούμε αν, πέρα από την ένταξη
	** της νέας στήλης, πρέπει να ενταχθεί στη λίστα `tbl_list'
	** και ο πίνακας που προφανώς εμφανίζεται πρώτη φορά.
	*/

	for (table = TBL_NULL, *p++ = 0, t = col_list;
		t != COL_NULL; t = t->next) {
		if (strcmp(s, t->table->name))
			continue;

		table = t->table;
		if (strcmp(p, t->column))
			continue;

		/*
		** Βρέθηκε το database column ήδη ενταγμένο στη
		** λίστα `col_list' και επιστρέφω τον αντίσοιχο
		** κόμβο χωρίς να κάνω καμία νέα εισαγωγή.
		*/

		return(t);
	}

	/*
	** Το database column δεν βρέθηκε στη λίστα και επομένως
	** θα το εντάξω τώρα. Ελέγχω πρώτα αν χρειάζεται να ενταχθεί
	** και ο πίνακας. Αν ο πίνακας εμφανίζεται πρώτη φορά τον
	** εντάσσω τώρα στη λίστα `tbl_list'.
	*/

	if (table == TBL_NULL)
		table = tbl_add(s, tlen);

	return(col_alloc(table, p, clen));
}
