#include "sparc.h"

/*
** Η function `sort_push' εισάγει μια μεταβλητή στο sort list.
** Η εισαγωγή γίνεται είτε άμεσα είτε έμμεσα· άμεση εισαγωγή έχω
** όταν παραθέτω το όνομα της μεταβλητής στο sort section, ενώ
** έμμεση εισαγωγή έχω όταν υπάρχει before/after event για την εν
** λόγω μεταβλητή χωρίς προηγουμένως να την έχω παραθέσει ρητά στο
** sort section. Η function δημιουργεί έναν νέο SORT node που αφορά
** στη μεταβλητή της οποίας τον pointer περνάμε ως πρώτη παράμετρο,
** με order type τον τύπο που περνάμε ως δεύτερη παράμετρο. Ο νέος
** κόμβος εντάσσεται τελευταίος στην global list `sort_list' (FIFO
** order). Παράλληλα τίθεται το πεδίο `input' του νέου κόμβου να είναι
** η θέση που κατέχει η μεταβλητή στο δευτερογενές (ταξινομημένο)
** input list. Η θέση αυτή είναι ίδια μ'αυτήν που κατείχε η εν λόγω
** μεταβλητή στο πρωτογενές input, εφόσον πρόκειται για μεταβλητή του
** πρωτογενούς input list, είτε η επόμενη στη σειρά θέση εφόσον
** πρόκειται για δευτερογενή μεταβλητή. Αυτή τη θέση την συντηρούμε
** στη local (static) μεταβλητή `input'. Παράλληλα, για λόγους
** αποτελεσματικότητας, συντηρούμε τον local (static) pointer
** `srt_last' που δείχνει πάντα τον τελευταίο κόμβο της global list
** `sort_list', ώστε να μην χρειάζεται κάθε φορά να διατρέχω το
** `sort_list' για να εντάξω νέο κόμβο και παράλληλα να διατηρώ το
** `sort_list' ως FIFO. Αυτό είναι σημαντικό γιατί μ'ενδιαφέρει η
** σειρά των sort fields. Κατά την κλήση της function και μετά την
** επιτυχή εισαγωγή νέου SORT node, αυξάνει ο global counter `n_sort'
** που δείχνει πόσα fields έχω στο sort list.
*/

SORT *sort_push(VARIABLE *v, int ord)
{
	SORT *p;
	static SORT *srt_last = SRT_NULL;
	static int input = 1;

	if (v == VAR_NULL)
		fatal("sort_push: null variable pointer "
			"(internal error)", EXIT_FATAL);

	if (v->sort) {
		yyerror(STR_NULL);
		fprintf(stderr, "%s: redefined sort field\n", v->name);
		return(SRT_NULL);
	}

	if ((void *)(p = (SORT *)malloc(sizeof(SORT))) == NULL)
		cht_nomem("srt_add", EXIT_MEMORY);

	p->var = v;
	p->ord = ord;
	p->before = TXT_NULL;
	p->bplist = PRM_NULL;
	p->after = TXT_NULL;
	p->aplist = PRM_NULL;
	p->next = SRT_NULL;
	if (!(p->input = p->var->input))
		p->input = n_input + (input++);

	if (srt_last == SRT_NULL)
		sort_list = (srt_last = p);
	else {
		srt_last->next = p;
		srt_last = p;
	}

	v->sort = ++n_sort;
	return(p);
}

static VARIABLE *var_lookup(VARIABLE *v)
{
	register VARIABLE *p;

	for (p = var_list; p != VAR_NULL; p = p->next) {
		if (v == p)
			break;
	}

	return(p);
}

/*
** Με τη `review_sort_fields' κάνουμε έναν επανέλεγχο των sort nodes
** στη `sort_list' και ελέγχουμε/διορθώνουμε διάφορα πράγματα. Ένα
** από αυτά είναι ο χαρακτηρισμός ascending όλων των implicit sort
** fields, δηλαδή όλων των sort fields που δηλώθηκαν ρητά ως ascending,
** descending ή none. Κατόπιν διατρέχουμε τη λίστα των input fields
** και εντάσσουμε στο sort list όλα τα input fields που δεν φαίνονται
** να μετέχουν στο sort· η ένταξη αυτών των fields γίνεται φυσικά με
** none order type.
*/

void review_sort_fields(void)
{
	int ns = 0;
	int max = 0;
	int errs = 0;
	SORT *p;
	int real_sort = 0;
	VARIABLE *v;

	for (p = sort_list; p != SRT_NULL; p = p->next, ns++) {
		if (var_lookup(p->var) == VAR_NULL) {
			cht_error("undefined sort field "
				"(internal error)");
			errs++;
		}

		/*
		** Τα sort fields που δεν έχουν καθορισμένο order type
		** τίθενται σιωπηλά ascending. Ακόμη, ελέγχονται τα
		** sort fields που δεν μετέχουν στο sort (πεδία
		** εμπλουτισμού) να μην έχουν before/after κώδικα.
		*/

		switch (p->ord) {
		case ORDER_UDF:
			p->ord = ORDER_ASC;
		case ORDER_ASC:
		case ORDER_DSC:
			real_sort++;
			break;
		case ORDER_NOT:
			if ((p->before != TXT_NULL) ||
				(p->after != TXT_NULL)) {
				cht_error(STR_NULL);
				fprintf(stderr, "%s: order not "
					"specified (before/after code "
					"ignored)\n", p->var->name);
				p->before = TXT_NULL;
				p->after = TXT_NULL;
			}
			break;
		default:
			cht_error(STR_NULL);
			fprintf(stderr, "%s: undefined sort order type "
				"(internal error)\n", p->var->name);
			errs++;
			break;
		}

		if (!(p->input)) {
			cht_error(STR_NULL);
			fprintf(stderr, "%s: undefined sort input "
				"position (internal error)\n",
				p->var->name);
			errs++;
			break;
		}

		if (p->var->sort > max)
			max = p->var->sort;
	}

	if (ns != n_sort) {
		cht_error(STR_NULL);
		fprintf(stderr, "%d sort fields vs %d sort nodes "
			"(internal error)\n", n_sort, ns);
		errs++;
	}

	if (max != n_sort) {
		cht_error(STR_NULL);
		fprintf(stderr, "%d: invalid sort fields' count "
			"(internal error)\n", max);
		errs++;
	}

	if (errs)
		exit(EXIT_FATAL);

	if (sorted < 0)
		sorted = 0;

	if ((!n_sort) && sorted) {
		fputs("`sorted' keyword ignored (useless "
			"without sort items)\n", stderr);
		sorted = 0;
	}

	if (!real_sort)
		sorted = 1;

	/*
	** Εντάσσω τώρα στο sort list όλα τα input fields που
	** ανήκουν ήδη στη sort list, καθώς, μετά το sort, θέλω
	** να έχω διαθέσιμα όλα τα sort και input fields.
	*/

	for (v = var_list; v != VAR_NULL; v = v->next) {
		if (v->input && (!(v->sort)))
			sort_push(v, ORDER_NOT);
	}

	if (sort_list == SRT_NULL)
		return;

	/*
	** Τώρα επεναδιατρέχω την sort list και την καθιστώ και
	** ανάστροφη θέτοντας τα πεδία `prev'.
	*/

	for (p = sort_list; p->next != SRT_NULL; p = p->next)
		p->next->prev = p;

	sort_last = p;
	sort_list->prev = SRT_NULL;
}
