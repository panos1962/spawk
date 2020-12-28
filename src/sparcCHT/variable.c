#include "sparc.h"
#include <string.h>

/*
** Η function `var_add' δέχεται ως πρώτη παράμετρο το όνομα μιας
** μεταβλητής και την εντάσσει στη λίστα `var_list' αν δεν είναι
** ήδη ενταγμένη. Επιστρέφει τον κόμβο που αφορά στην εν λόγω
** μεταβλητή είτε αυτή υπήρχε ήδη στη λίστα είτε εντάχθηκε τώρα.
** Ως δεύτερη παράμετρο περνάω το μήκος του ονόματος της μεταβλητής
** το οποίο είναι συνήθως γνωστό τη στιγμή που καλείται η function·
** αν περάσω μηδενικό μήκος τότε αυτό σημαίνει ότι το όνομα είναι
** ήδη κρατημένο ως string (έχει κρατηθεί μέσω της `col_add'
** προηγουμένως) οπότε ο VARIABLE κόμβος που ίσως δημιουργηθεί θα
** δείχνει στο ήδη κρατημένο όνομα.
*/

VARIABLE *var_add(char *name, int len)
{
	VARIABLE *v;

	if (name == STR_NULL)
		fatal("var_add: null variable name", EXIT_FATAL);

	for (v = var_list; v != VAR_NULL; v = v->next) {
		if (!strcmp(name, v->name))
			return(v);
	}

	if (((void *)(v = (VARIABLE *)
		malloc(sizeof(VARIABLE)))) == NULL)
		cht_nomem("var_alloc", EXIT_MEMORY);

	if (len > 0)
		v->name = cht_strnsave(name, len, EXIT_MEMORY);
	else
		v->name = name;

	v->input = 0;
	v->sort = 0;
	v->type = 0;
	v->column = COL_NULL;
	v->next = var_list;
	return(var_list = v);
}

/*
** Η function `review_varables' καλείται πριν αρχίσει η παραγωγή
** του awkrpt κώδικα και ελέγχει ποιές από τις μεταβλητές που
** εμφανίζονται στο sort ή στο input δεν έχουν καθορισμένο (άμεσα
** ή έμμεσα) data type. Αυτές οι μεταβλητές τίθενται αυτόματα
** αλφαριθμητικές (string), αλλά τυπώνονται και σχετικά μηνύματα
** κατά το compilation time, δηλαδή τώρα.
*/

void review_variables(void)
{
	VARIABLE *v;
	int errs = 0;

	for (v = var_list; v != VAR_NULL; v = v->next) {
		if (v->name == STR_NULL) {
			cht_error("undefined variable name "
				"(internal error)");
			errs++;
		}

		if (v->type)
			continue;

		if (v->column != COL_NULL)
			continue;

		cht_error(STR_NULL);
		fprintf(stderr, "%s: data type implicitly set "
			"to `string'\n", v->name);
		v->type = TYPE_STRING;
	}

	if (errs)
		exit(EXIT_FATAL);
}
