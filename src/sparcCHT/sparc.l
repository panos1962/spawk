%{

#include <ctype.h>
#include <string.h>
#include "sparc.h"
#include "sparc.tab.h"

static int kwd_lookup(char *);
static TEXT *txt_read(void);
static int fname_check(void);
static PARAMETER *prm_alloc(char *, int);

%}

STRING	[a-zA-Z_][a-zA-Z0-9_]*
NUMBER	[0-9][0-9]*

%%

\%%.*\n {	// awk code (or other text)
	lineno++;
	yylval.txt = txt_read();
	return(AWK_CODE);
}

(#|\/\/).*$ {	// ignore comments
	;
}

[ \t] {		// ignore "white" characters
	;
}

{STRING} {	// keyword or variable name
	int kwd;

	if (plist_on) {
		yylval.prm = prm_alloc(yytext, yyleng);
		return(PRMNAME);
	}

	if (kwd = kwd_lookup(yytext))
		return(kwd);

	if (aacs_type) {
		yylval.fname = cht_strnsave(yytext, yyleng,
			EXIT_MEMORY);
		return(AACS);
	}

	yylval.var = var_add(yytext, yyleng);
	return(VARNAME);
}

{STRING}\.{STRING} {	// database column name
	yylval.col = col_add(yytext, yyleng);
	return(COLNAME);
}

['"].*['"] {
	if (*yytext != (*(yytext + yyleng - 1))) {
		yyerror(yytext);
		return(0);
	}

	if (!aacs_type) {
		yyerror(STR_NULL);
		fprintf(stderr, "%s: unexpected archive/application/"
			"command/options\n", yytext);
		return(0);
	}

	yylval.fname = cht_strnsave(yytext + 1, yyleng - 2,
		EXIT_MEMORY);
	return(AACS);
}

{NUMBER} {
	yylval.num = atoi(yytext);
	return(NUMBER);
}

\n {		// count input lines
	lineno++;
}

. {		// return ASCII code of this character as token
	return(*yytext);
}

%%

// Ακολουθεί λίστα με τα keywords του προγράμματος. Τα περισσότερα
// από αυτά επιστρέφονται αυτούσια στον parser. Εξαίρεση αποτελούν
// τα data type keywords που επιστρέφονται ως VARTYPE tokens στον
// parser έχοντας τιμή (yylval.dtype) που δείχνει τον εν λόγω data
// type.

static struct {
	char *keyword;		// token value
	int token;		// 0 = data type, else token value
	int dtype;		// data type code for data types
	int aac;		// application/archive/command/script
} kwd_table[] = {
	"before",	BEFORE_KWD,		0,		0,
	"after",	AFTER_KWD,		0,		0,
	"numeric",	0,			TYPE_NUMERIC,	0,
	"string",	0,			TYPE_STRING,	0,
	"date",		0,			TYPE_DATE,	0,
	"float",	0,			TYPE_FLOAT,	0,
	"time",		0,			TYPE_TIME,	0,
	"sorted",	SORTED_KWD,		0,		0,
	"input",	INPUT_KWD,		0,		0,
	"sort",		SORT_KWD,		0,		0,
	"detail",	DETAIL_KWD,		0,		0,
	"begin",	BEGIN_KWD,		0,		0,
	"end",		END_KWD,		0,		0,
	"any",		ANY_KWD,		0,		0,
	"accepted",	ACCEPTED_KWD,		0,		0,
	"data",		DATA_KWD,		0,		0,
	"set",		SET_KWD,		0,		0,
	"report",	REPORT_KWD,		0,		0,
	"awkward",	AWKWARD_KWD,		0,		0,
	"functions",	AWKWARD_KWD,		0,		0,
	"function",	FUNCTION_KWD,		0,		0,
	"script",	SCRIPT_KWD,		0,    AACS_SCRIPT,
	"program",	SCRIPT_KWD,		0,    AACS_SCRIPT,
	"archives",	ARCHIVES_KWD,		0,      AACS_ARCH,
	"archive",	ARCHIVES_KWD,		0,      AACS_ARCH,
	"applications",	APPLICATIONS_KWD,	0,      AACS_APPL,
	"application",	APPLICATIONS_KWD,	0,      AACS_APPL,
	"format",	FORMAT_KWD,		0,		0,
	"post",		POST_KWD,		0,       AACS_CMD,
	"ante",		ANTE_KWD,		0,       AACS_CMD,
	"feed",		FEED_KWD,		0,       AACS_CMD,
	"options",	OPTIONS_KWD,		0,       AACS_OPT,
	"option",	OPTIONS_KWD,		0,       AACS_OPT,
	"asc",		'+',			0,		0,
	"dsc",		'-',			0,		0,
	"ascending",	'+',			0,		0,
	"descending",	'-',			0,		0,
	"none",		'*',			0,		0,
	"skip",		SKIP_KWD,		0,		0,
};

// Η function `kwd_lookup' διατρέχει το keyword list και αν εντοπίσει
// το string που της περνάμε ως παράμετρο, ελέγχει αν πρόκειται για
// συγκεκριμένο token, οπότε το επιστρέφει στον parser, αλλιώς
// επιστρέφει VARTYPE token με τιμή (yylval.dtype) τον αντίστοιχο data
// type. Αν δεν εντοπιστεί το string στο keyword list, τότε η function
// επιστρέφει μηδέν οπότε θα επιστρέψω token VARNAME στον parser.

static int kwd_lookup(char *s)
{
	int i;

	for (i = 0; i < sizeof(kwd_table) / sizeof(*kwd_table); i++) {
		if (strcmp(s, kwd_table[i].keyword))
			continue;

		if (kwd_table[i].dtype) {
			yylval.typ = kwd_table[i].dtype;
			return(VARTYPE);
		}

		aacs_type = kwd_table[i].aac;
		return (kwd_table[i].token);
	}

	return 0;
}

static TEXT *txt_include(TEXT *t, char *fname)
{
	FILE *fp;
	char buf[MAX_LINE_LENGTH + 2];
	char *p;
	int c;

	// Θα δούμε τώρα μήπως το include file name έχει δοθεί
	// μέσα σε quotes (μονά ή διπλά).

	switch (*fname) {
	case '\'':
	case '"':
		for (p = fname + 1; *p; p++)
			;
		if ((p-- > fname + 1) && (*p == *fname)) {
			*p = 0;
			fname++;
		}
	}

	if ((fp = fopen(fname, "r")) == (FILE *)(NULL)) {
		yyerror(STR_NULL);
		perror(fname);
		return(TXT_NULL);
	}

READ_TEXT:
	for (p = buf; (c = getc(fp)) != EOF;) {
		*p++ = c;
		if (p - buf >= MAX_LINE_LENGTH)
			break;
	}

	*p = 0;
	t = txt_add(t, buf, p - buf);
	if (c != EOF)
		goto READ_TEXT;

	fclose(fp);
	return(t);
}

// Όπου συναντήσω στο input γραμμή της μορφής "%%" σημαίνει ότι
// ακολουθεί awk κώδικας. Ο κώδικας κλείνει πάλι με μια γραμμή
// της μορφής "%%", οπότε διαβάζω μέχρι εκεί (η το τέλος του input)
// και αποθηκεύω τον περιεχόμενο κώδικα σε TEXT list. Τέλος, επιστρέφω
// στον parser AWK_CODE token με τιμή (yylval.txt) το TEXT list που
// δημιουργήθηκε.
//
// Το σύμβολο "%%" μπορεί να χρησιμοποιηθεί και συμπερίληψη (include)
// αρχείων που περιέχουν awk κώδικα, π.χ.
//
//   ...
//   %% /cht/awk/prosop/adia/isigisi1
//   ...

static int directive(char *p, char **fname)
{
	char *q;

	// Έχουμε δεχτεί γραμμή που ξεκινάει με "%%".
	// Ελέγχουμε αν πρόκειται για file inclusion
	// ή απλά για awk code begin/end. Ξεκινάμε
	// "τρώγοντας" τους "λευκούς" χαρακτήρες.
	// Επίσης θέτουμε αρχικά το file name pointer
	// που περνάμε ως παράμετρο σε null καθώς
	// αυτός ο pointer θα μας καθοδηγήσει κατά
	// την επιστροφή αν επρόκειτο για file inclusion
	// ή απλά για awk code begin/end. Η fucntion
	// επιστρέφει μη μηδενική τιμή για λάθος,
	// αλλιώς επιστρέφει μηδέν.

	for (*fname = STR_NULL, p += 2; *p; p++) {
		switch(*p) {
		case ' ':
		case '\t':
			continue;
		case '\n':
		case '#':
			return(0);
		}

		break;
	}

	for (q = p; *q; q++) {
		if (*q == '/') {
			if (*(q + 1) == '/') {
				*q = 0;
				break;
			}
		}
	}

	if (!(*p))
		return(0);

	// Έχουμε εντοπίσει file name και ψάχνουμε τώρα να δούμε
	// πού τελειώνει. Τέλος θεωρείται το τέλος του string και
	// οι λευκοί χαρακτήρες.

	for (*fname = p; *p; p++) {
		switch (*p) {
		case ' ':
		case '\t':
			*p++ = 0;
			break;
		case '\n':
			*p = 0;
			return(0);
		}
	}

	if (!(*p))
		return(0);

	// Έχουμε τελειώσει με το file name αλλά φαίνεται να
	// υπάρχει συνέχεια. Η μόνη δεκτή συνέχεια είναι και
	// άλλοι λευκοί χαρακτήρες ή σχόλιο.

	for (; *p; p++) {
		switch (*p) {
		case ' ':
		case '\t':
			continue;
		case '\n':
		case '#':
			return(0);
		}

		yyerror("invalid file inclusion");
		return(1);
	}

	return(0);
}

static char *directive_line(char *s)
{
	while (*s) {
		switch (*s) {
		case ' ':
		case '\t':
			s++;
			break;
		default:
			return (strncmp(s, "%%", 2) ? STR_NULL : s);
		}
	}

	return(STR_NULL);
}

static TEXT *txt_read(void)
{
	char buf[MAX_LINE_LENGTH + 2];
	TEXT *t = TXT_NULL;
	int c;
	char *p;
	char *q;
	char *fname;

	if (directive(yytext, &fname))
		return(TXT_NULL);

	if (fname != STR_NULL)
		return (strcmp(fname, "%%") ?
			txt_include(t, fname) : TXT_NULL);

	// Δεν επρόκειτο για file inclusion και απλά έχω awk
	// κώδικα μέχρι να συναντήσω εκ νέου "%%". Μέσα όμως
	// στον κώδικα που ακολουθεί μπορεί να υπάρχει file
	// inclusion.

READ_TEXT:
	for (p = buf; (c = input()) != EOF;) {
		*p++ = c;
		if (c == '\n') {
			lineno++;
			break;
		}

		if ((p - buf) >= MAX_LINE_LENGTH)
			cht_fatal("huge awk souce code line",
				EXIT_FATAL);
	}

	*p = 0;
	if ((q = directive_line(buf)) != STR_NULL) {
		if (directive(q, &fname) || (fname == STR_NULL))
			return(t);

		t = txt_include(t, fname);
		goto READ_TEXT;
	}

	t = txt_add(t, buf, p - buf);
	if (c != EOF)
		goto READ_TEXT;

	return(t);
}

static PARAMETER *prm_alloc(char *pname, int len) {
	PARAMETER *prm;

	if ((void *)(prm = (PARAMETER *)
		malloc(sizeof(PARAMETER))) == NULL)
		cht_nomem("prm_alloc", EXIT_MEMORY);

	prm->name = cht_strnsave(pname, len, EXIT_MEMORY);
	prm->next = PRM_NULL;
	return(prm);
}
