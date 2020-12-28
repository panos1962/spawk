%{

#include "sparc.h"
#include <string.h>

#define add_code(plist, fcode, code) { \
	plist = plist_more(plist, awk_plist); \
	fcode = txt_more(fcode, code); \
}

extern int yylex();

// Η function `input_push' εισάγει τη μεταβλητή, της οποίας τον
// pointer περνάμε ως πτώτη παράμετρο, στο input field list.
// Στην πραγματικότητα δεν υπάρχει κανένα υλοποιημένο input field
// list, απλά μαρκάρεται η μεταβλητή ως πεδίο του αρχικού input
// θέτοντας το πεδίο `input' της μεταβλητής στο επόμενο input field
// σύμφωνα με τον global counter `n_input' ο οποίος αυξάνεται κατά
// ένα ακριβώς σ'αυτό το σημείο.

static void input_push(VARIABLE *v)
{
	if (v == VAR_NULL)
		fatal("input_push: null variable pointer "
			"(internal error)", EXIT_FATAL);

	if (v->input) {
		yyerror(STR_NULL);
		fprintf(stderr, "%s: redefined input field\n", v->name);
		return;
	}

	v->input = ++n_input;
}

//
// Η `cur_plist' δείχνει την τρέχουσα λίστα τοπικών μεταβλητών.
// Κάθε awk code segment μπορεί να έχει local variables, καθώς
// τα event sections' awk code segments υλοποιούνται τελικά
// ως awk functions. Η λίστα τοπικών μεταβλητών γράφεται συνήθως
// πριν το awk code segment που συνοδεύει κάποιο event. Οι local
// variables γράφονται μέσα σε παρενθέσεις και είναι είτε comma
// separated list, είτε απλή λίστα ("λευκοί" separators). Για
// ταχύτητας διατηρούμε πάντα και την τρέχουσα τελευταία παράμετρο
// της τρέχουσας λίστας (`last_prm').

static PARAMETER *cur_plist = PRM_NULL;
static PARAMETER *last_prm = PRM_NULL;
static PARAMETER *awk_plist = PRM_NULL;

// Η function `push_prm' προσθέτει νέα local variable στην τρέχουσα
// local variables' list (`cur_plist').

static void push_prm(char *pname) {
	PARAMETER *prm;

	if ((void *)(prm = (PARAMETER *)malloc(sizeof(PARAMETER))) ==
		NULL)
		cht_nomem("push_prm", EXIT_MEMORY);

	prm->name = pname;
	prm->next = PRM_NULL;
	if (last_prm != PRM_NULL) {
		last_prm->next = prm;
		last_prm = prm;
	}
	else
		last_prm = (cur_plist = prm);

}

// Η function `plist_more' προσθέτει νέες local variables σε
// προϋπάρχουσα local variables' list.

static PARAMETER *plist_more(PARAMETER *p1, PARAMETER *p2) {
	PARAMETER *prm;

	if (p2 == PRM_NULL)
		return(p1);

	if (p1 == PRM_NULL)
		return(p2);

	for (prm = p1; prm->next != PRM_NULL;)
		prm = prm->next;

	prm->next = p2;
	return(p1);
}

// Η function `bac_add' προσθέτει κώδικα σε before/after field events.
// Ως πρώτη παράμετρο περνάμε pointer στη μεταβλητή που αφορά στο εν
// λόγω event, ως δεύτερη παράμετρο περνάμε μη μηδενική τιμή για
// `before' ή μηδέν για `after' event, και ως τρίτη παράμετρο περνάμε
// το order type που ίσως καθορίστηκε implicitly στο εν λόγω
// before/after event. Αν υπάρχει ήδη κώδικας στο εν λόγω event,
// ο κώδικας που περνάμε προστίθεται (append) στον υπάρχοντα κώδικα.

static void bac_add(VARIABLE *v, TEXT *txt, const int before, int ord)
{
	register SORT *p;

	if (!v->sort) {
		warning(STR_NULL);
		fprintf(stderr, "%s: implicit sort field\n", v->name);
		p = sort_push(v, (ord == ORDER_UDF ? ORDER_ASC : ord));
		goto BEFORE_AFTER_CODE;
	}

	// Εντοπίζω τη μεταβλητή στο sort list. Εφόσον η μεταβλητή
	// υπάρχει, θα ελεγχθεί και το order type εφόσον αυτό
	// καθορίζεται στην παρούσα κλήση.

	for (p = sort_list; p != SRT_NULL; p = p->next) {
		if (p->var != v)
		continue;

		if (ord == ORDER_UDF)
		break;

		if (ord == p->ord)
		break;

		warning(STR_NULL);
		fprintf(stderr, "%s: sort order redefinition\n", p->var->name);
		p->ord = ord;
		break;
	}

BEFORE_AFTER_CODE:
	if (before) {
		p->bplist = plist_more(p->bplist, awk_plist);
		p->before = txt_more(p->before, txt);
	}
	else {
		p->aplist = plist_more(p->aplist, awk_plist);
		p->after = txt_more(p->after, txt);
	}
}

// Η function `set_var_type' δέχεται ως πρώτη παράμετρο έναν VARIABLE
// pointer και θέτει τον τύπο της αντίστοιχης μεταβλητής στον τύπο
// που καθορίζουμε ως δεύτερη παράμετρο, εφόσον αυτή είναι μη μηδενική.
// Αν ο τύπος που περνάμε είναι μηδενικός, τότε ο τύπος τίθεται έμμεσα
// στον τύπο της database column της οποίας τον pointer περνάμε ως
// τρίτη παράμετρο.

static VARIABLE *set_var_type(VARIABLE *v, int typ, DB_COLUMN *col)
{
	if (v == VAR_NULL)
	fatal("set_var_type: null variable pointer (internal error)", EXIT_FATAL);

	if (typ && (col != COL_NULL))
	fatal("set_var_type: both type and column specified (internal error)", EXIT_FATAL);

	if ((!typ) && (col == COL_NULL))
	fatal("set_var_type: neither type nor column specified (internal error)", EXIT_FATAL);

	if (
		(typ &&
			(
				(v->type && (v->type != typ)) ||
				(v->column != COL_NULL)
			)
		) ||
		((col != COL_NULL) &&
			(
				v->type ||
				((v->column != COL_NULL) &&
					(v->column != col))
			)
		)
	) {
		yyerror(NULL);
		fprintf(stderr, "%s: data type redefinition\n", v->name);
	}

	v->type = typ;
	v->column = col;
	return(v);
}

static int flist_type = 0;
static int plist_na = 0;

static void awk_function(char *fname, TEXT *code) {
	char buf[MAX_LINE_LENGTH + 2];
	PARAMETER *prm;

	sprintf(buf, "\nfunction %s(", fname);
	functions_code = txt_add(functions_code, buf, strlen(buf));
	if (awk_plist != PRM_NULL) {
		sprintf(buf, "%s", awk_plist->name);
		txt_add(functions_code, buf, strlen(buf));
		for (prm = awk_plist->next; prm != PRM_NULL;
			prm = prm->next) {
			sprintf(buf, ", %s", prm->name);
			txt_add(functions_code, buf, strlen(buf));
		}
	}

	strcpy(buf, ") {\n");
	txt_add(functions_code, buf, strlen(buf));
	txt_more(functions_code, code);
	strcpy(buf, "}\n");
	txt_add(functions_code, buf, strlen(buf));
}

static void input_push(VARIABLE *);
static PARAMETER *plist_more(PARAMETER *, PARAMETER *);
static VARIABLE *set_var_type(VARIABLE *, int, DB_COLUMN *);

%}

// Η parser stack μπορεί να περιέχει τα εξής αντικείμενα:
//
// TEXT pointer
// ------------
// Πρόκειται για κείμενο που ουσιαστικά περιέχει awk κώδικα που
// εμφανίζεται σε διάφορα σημεία του awkrpt script.
//
// VARIABLE pointer
// ----------------
// Όλες οι μεταβλητές που εμφανίζονται στο script (όχι αυτές που
// βρίσκονται μέσα σε awk κώδικα, αλλά αυτές που μπορεί να αναγνωρίσει
// ο akwrpt compiler) τοποθετούνται (μοναδικά) σε VARIABLE nodes
// και τις χειριζόμαστε με τους αντίστοιχους pointers.
//
// DB_COLUMN pointer
// -----------------
// Όπου αναγνωριστεί database column name κατά τη φάση του lexical
// analyzing, η στήλη τοποθετείται (μοναδικά) σε DB_COLUMN node.
// Μέσα στο πρόγραμμα χειριζόμαστε τα database columns ως pointers
// των παραπάνω κόμβων.
//
// Data Type
// ---------
// Πρόκειται ουσιαστικά για ακέραιο αριθμό που δείχνει τον data type
// και αναγνωρίζεται από τον lexical analyzer ο οποίος το επιστρέφει
// ως token τύπου `VARTYPE' με τιμή τον εν λόγω τύπο.
//
// Sort Order Type
// ---------------
// Πρόκειται για ακέραιο αριθμό που δείχνει το είδος του sort που
// επιθυμώ σε διάφορα sort fields. Οι τύποι είναι ASCENDING (1),
// DESCENTING (2) NONE (3) και UNDEFINED (-1). Ως tokens επιστρέφονται
// από τον lexical analyzer οι χαρακτήρες (+), (-) και (*) για
// ascending, descenting και none order type αντίστοιχα. Ο lexical
// analyzer αναγνωρίζει επίσης και τα keywords "ascending", "asc",
// "descenting" και "dsc" επιστρέφοντας πάντα τους χαρακτήρες (+) ή (-).
//
// Parameters' list
// ----------------
// Pointer σε local variables' list. Οι local variables που
// καθορίζονται στα διάφορα awk code segments σχηματίζουν λίστες
// και ως τέτοιες τις διαχειριζομαστε. Δηλαδή κάθε awk code
// segment έχει και την αντίστιοιχη local variables' list.
//
// Λήφθηκε ιδιαίτερη μέριμνα ώστε το stack union να περιέχει μόνο
// αριθμητικούς τύπους (int ή pointers). Αυτό είναι πολύ σημαντικό
// και για την απλότητα του κώδικα (τα αντικείμενα εντοπίζονται
// ευκολότερα) και για την αποτελεσματικότητα του compiler (μικρό
// και ευέλικτο stack).

%union {
	TEXT *txt;		/* awk source code (as text) */
	VARIABLE *var;		/* variable */
	DB_COLUMN *col;		/* database column */
	int typ;		/* variable type (as number) */
	int ord;		/* sorting order (ascend/descend) */
	char *fname;		/* file name */
	int num;		/* number value */
	PARAMETER *prm;		/* section parameters' list */
}

%start program
%token SORTED_KWD
%token INPUT_KWD
%token SORT_KWD
%token BEGIN_KWD
%token END_KWD
%token BEFORE_KWD
%token AFTER_KWD
%token REPORT_KWD
%token ANY_KWD
%token ACCEPTED_KWD
%token DATA_KWD
%token SET_KWD
%token DETAIL_KWD
%token AWKWARD_KWD
%token FUNCTION_KWD
%token SCRIPT_KWD
%token ARCHIVES_KWD
%token APPLICATIONS_KWD
%token OPTIONS_KWD
%token FORMAT_KWD
%token POST_KWD
%token ANTE_KWD
%token FEED_KWD
%token SKIP_KWD
%token <fname> AACS
%type <fname> aacs
%token <col> COLNAME
%token <txt> AWK_CODE
%token <var> VARNAME
%token <typ> VARTYPE
%token <num> NUMBER
%token <prm> PRMNAME
%type <prm> prmname
%type <prm> prm_list
%type <prm> pplist
%type <prm> vlist
%type <prm> plist
%type <txt> awk_code
%type <var> variable
%type <ord> asc_dsc

%%

program
	: {
		exit(EXIT_OK);
	}
	| sections
	;

sections
	: section
	| sections section {
		yyerrok;
	}
	| sections error {
		n_errs++;
	}
	;

section
	: declaration
	| input
	| SORT_KWD slist
	| event
	;

input
	: INPUT_KWD ilist {
		if (sorted == -1)
		sorted = 0;
	}
	| SORTED_KWD INPUT_KWD ilist {
		if (!sorted)
		warning("previous input considered sorted also");

		sorted = 1;
	}
	;

ilist
	:
	| ilist variable {
		yyerrok;
		input_push($2);
	}
	| ilist SKIP_KWD NUMBER {
		yyerrok;
		n_input += $3;
	}
	| ilist SKIP_KWD {
		yyerrok;
		n_input++;
	}
	| ilist pplist {
		yyerrok;
		input_plist = plist_more(input_plist, $2);
	}
	| ilist AWK_CODE {
		yyerrok;
		input_code = txt_more(input_code, $2);
	}
	| ilist error {
		n_errs++;
	}
	;

slist
	:
	| slist variable asc_dsc {
		yyerrok;
		sort_push($2, $3);
	}
	| slist variable asc_dsc '[' VARTYPE ']' {
		yyerrok;
		sort_push(set_var_type($2, $5, COL_NULL), $3);
	}
	| slist variable asc_dsc '[' COLNAME ']' {
		yyerrok;
		sort_push(set_var_type($2, 0, $5), $3);
	}
	| slist error {
		n_errs++;
	}
	;

event
	: BEGIN_KWD awk_code {
		add_code(begin_plist, begin_code, $2);
	}
	| BEFORE_KWD REPORT_KWD awk_code {
		add_code(before_report_plist, before_report_code, $3);
	}
	| BEFORE_KWD data awk_code {
		add_code(before_first_data_plist, before_first_data_code, $3);
	}
	| BEFORE_KWD accepted_data awk_code {
		add_code(before_accepted_data_plist, before_accepted_data_code, $3);
	}
	| BEFORE_KWD data_set awk_code {
		add_code(before_data_plist, before_data_code, $3);
	}
	| BEFORE_KWD variable asc_dsc awk_code {
		bac_add($2, $4, 1, $3);
	}
	| DETAIL_KWD awk_code {
		add_code(detail_plist, detail_code, $2);
	}
	| AFTER_KWD SORT_KWD awk_code {
		add_code(after_sort_plist, after_sort_code, $3);
	}
	| AFTER_KWD variable asc_dsc awk_code {
		bac_add($2, $4, 0, $3);
	}
	| AFTER_KWD data_set awk_code {
		add_code(after_data_plist, after_data_code, $3);
	}
	| AFTER_KWD data awk_code {
		add_code(after_last_data_plist, after_last_data_code, $3);
	}
	| AFTER_KWD REPORT_KWD awk_code {
		add_code(after_report_plist, after_report_code, $3);
	}
	| AFTER_KWD accepted_data awk_code {
		add_code(after_accepted_data_plist, after_accepted_data_code, $3);
	}
	| END_KWD awk_code {
		add_code(end_plist, end_code, $2);
	}
	;

data
	: DATA_KWD
	| ANY_KWD DATA_KWD
	;

accepted_data
	: ACCEPTED_KWD DATA_KWD
	| ANY_KWD ACCEPTED_KWD DATA_KWD
	| ACCEPTED_KWD
	;

data_set
	: SET_KWD
	| DATA_KWD SET_KWD
	;

variable
	: VARNAME {
		$$ = $1;
	}
	| VARNAME '[' VARTYPE ']' {
		$$ = set_var_type($1, $3, COL_NULL);
	}
	| VARNAME '[' COLNAME ']' {
		$$ = set_var_type($1, 0, $3);
	}
	| COLNAME {
		$$ = set_var_type(var_add($1->column, 0), 0, $1);
	}
	;

declaration
	: SCRIPT_KWD aacs {
		if ((script_name != STR_NULL) &&
			strcmp($2, script_name)) {
			warning(STR_NULL);
			fprintf(stderr, "%s: script name redefinition (%s)\n", $2, script_name);
			free(script_name);
		}

		script_name = $2;
	}
	| AWKWARD_KWD {
		plist_na = 1;
	} awk_code {
		functions_code = txt_more(functions_code, $3);
		plist_na = 0;
	}
	| FUNCTION_KWD {
		plist_on = 1;
	} prmname {
		plist_on = 0;
	} awk_code {
		if ($3 != PRM_NULL)
			awk_function($3->name, $5);
		else {
			functions_code = txt_add(functions_code, "{\n", 2);
			functions_code = txt_more(functions_code, $5);
			functions_code = txt_add(functions_code, "}\n", 2);
			plist_na = 0;
		}
	}
	| ARCHIVES_KWD flist
	| APPLICATIONS_KWD flist
	| OPTIONS_KWD flist
	| FORMAT_KWD {
		tpm_format = 1;
	}
	| POST_KWD aacs {
		post_program = post_ante(post_program, $2);
	}
	| POST_KWD FORMAT_KWD {
		aacs_type = AACS_CMD;
	} aacs {
		post_program = post_ante(post_program, $4);
		tpm_format = 1;
	}
	| ANTE_KWD aacs {
		ante_program = post_ante(ante_program, $2);
	}
	| FEED_KWD aacs {
		ante_program = post_ante(ante_program, $2);
		ante_option = 'A';
	}
	;

	// Ως `awk_code' ορίζεται μια οντότητα η οποία μπορεί να
	// είναι είτε κενή, είτε μια local variables' list (που
	// επίσης μπορεί να μην υπάρχει) συνοδευόμενη από awk
	// κώδικα. Άρα, όπου μπορεί να εμφανιστεί awk κώδικας,
	// δίνουμε την οντότητα `awk_code'. Σε κάποιες περιπτώσεις
	// (π.χ. στα `awkward' sections) δεν επιτρέπεται local
	// variables' list. Αυτό σηματοδοτείται από τη flag `plist_na'.

awk_code
	: plist {
		awk_plist = $1;
		$$ = TXT_NULL;
	}
	| awk_code AWK_CODE {
		yyerrok;
		$$ = txt_more($1, $2);
	}
	;

prmname
	: {
		plist_na = 1;
		$$ = PRM_NULL;
	}
	| PRMNAME {
		$$ = $1;
	}
	;

asc_dsc
	: {
		$$ = ORDER_UDF;		// unspecified order type
	}
	| '+' {
		$$ = ORDER_ASC;		// ascending order
	}
	| '-' {
		$$ = ORDER_DSC;		// descending order
	}
	| '*' {
		$$ = ORDER_NOT;		// no order - no real sort
	}
	;

flist
	: {
		// Στην αρχή ενός `flist' κρατάω το τρέχον
		// aacs_type. Αυτό το κάνω διότι το accs_type
		// τίθεται από τον lexical analyzer σύμφωνα με
		// το keyword table. Επομένως, κατά το κλείσιμο
		// της `flist' έχω ήδη πάρει κάποιο άλλο token
		// και πιθανώς να έχω αλλάξει το accs_type.

		flist_type = aacs_type;
	}
	| flist aacs {
		yyerrok;
		switch (flist_type) {
		case AACS_ARCH:
		case AACS_APPL:
		case AACS_OPT:
			add_archive($2, flist_type);
		}
	}
	| flist error {
		n_errs++;
	}
	;

aacs
	: AACS {
		$$ = $1;
	}
	| aacs '+' AACS {
		char *p;
		int l;

		if (($$ = (char *)malloc(strlen($1) + strlen($3) + 1)) == STR_NULL)
		cht_nomem("concatenating aacs strings", EXIT_MEMORY);

		p = cht_strcpy($$, $1);
		strcpy(p, $3);
		free($1);
		free($3);
	}
	;

	// Η οντότητα `plist' είναι μια local parameters' list
	// που αφορά σε κάποιο event awk code segment. Η οντότητα
	// αυτή μπορεί να υπάρχει, μπορεί και όχι. Ως `plist'
	// ορίζεται ακριβώς αυτό δηλαδή κάτι που είτε είναι απόν,
	// είτε είναι μια local parametrs' list.

plist
	: {
		$$ = PRM_NULL;
	}
	| vlist {
		$$ = $1;
	}
	;

	// Ως `vlist' ορίζεται η local parameters' list εφόσον
	// όμως αυτή είναι υπαρκτή. Συνήθως μια τέτοια λίστα
	// μεταβλητών είναι μια λίστα ονομάτων μέσα σε παρενθέσεις.
	// Μπορεί όμως να έχω τέτοιου είδους λίστες στη σειρά, π.χ.
	// (a, b, c) (x, y), που ουσιαστικά συνθέτουν τη λίστα:
	// (a, b, c, x, y). Για το λόγο αυτό, οι λίστες ονομάτων
	// μέσα σε παρενθέσεις ορίζονται (παρακάτω) ως `pplist'
	// οντότητες, και ως `vlist' ορίζουμε ακριβώς μια σειρά
	// από `pplist' λίστες.

vlist
	: pplist {
		$$ = $1;
	}
	| vlist pplist {
		$$ = plist_more($1, $2);
	}
	;

	// Φθάσαμε πια στο σημείο να ορίσουμε μια υπαρκτή local
	// parameters' list. Πρόκειται για μια σειρα ονομάτων
	// (`prm_list') μέσα σε παρενθέσεις.

pplist
	: '(' {
		if (plist_na)
		warning("parameters not allowed (ignored)");
	} prm_list ')' {
		$$ = (plist_na ? PRM_NULL : $3);
		plist_on = 0;		// emanicipate lexical analyzer
	}
	;

	// Οι local parameter lists αποτελούνται από ονόματα
	// μεταβλητών χωρισμένα μεταξύ τους είτε με κόμματα,
	// είτε με "λευκούς" χαρακτήρες.

prm_list
	: {
		$$ = PRM_NULL;
		plist_on = 1;		// pledge lexical analyzer
	}
	| prm_list PRMNAME {
		yyerrok;
		$$ = plist_more($1, $2);
	}
	| prm_list ',' {
		yyerrok;
	}
	| prm_list error {
		n_errs++;
	}
	;

%%
