#include <stdio.h>
#include <stdlib.h>
#include "cht.h"

#define EXIT_OK 0
#define EXIT_USAGE 1
#define EXIT_FATAL 2
#define EXIT_MEMORY 3
#define EXIT_SYNTAX 5
#define EXIT_DB_COLUMN 6
#define EXIT_INPUT 7
#define EXIT_SORT 8
#define EXIT_OBJECT 9
#define EXIT_REPORT 10

#define MAX_LINE_LENGTH 16382
#define MAX_VARIABLE_COUNT 1024
#define MAX_INPUT_COUNT 1024
#define MAX_SORT_COUNT 1024
#define MAX_FILE_NAME_LENGTH 4000
#define MAX_COMMAND_LENGTH 8000

/*
** Η structure `TEXT' χρησιμοποιείται για την αποθήκευση κειμένου.
** Ως κείμενο θεωρείται και ο AWK κώδικας που εμφανίζεται μέσα στα
** awkrpt scripts. Η structure αποτελείται από ένα κομμάτι δεδομένων
** (όχι απαραίτητα ολοκληρωμένη γραμμή), έναν pointer στο επόμενο
** κομμάτι και έναν pointer στο τελευταίο κομμάτι του εν λόγω κειμένου.
*/

typedef struct TEXT {
	char *text;			/* text data segment */
	struct TEXT *next;		/* next text segment */
	struct TEXT *last;		/* last segment of this text */
} TEXT;

/*
** Η structure `DB_TABLE' κρατάει τους πίνακες της database που
** εμφανίζονται σε διάφορα database columns. Για τους πίνακες αυτούς
** θα παραχθούν εντολές φόρτωσης των τύπων των πεδίων τους στο
** awkrpt script.
*/

typedef struct DB_TABLE {
	char *name;
	struct DB_TABLE *next;
} DB_TABLE;

/*
** Η structure `DB_COLUMN' παριστάνει ένα database column. Αποτελείται
** από το όνομα του πίνακα και το όνομα του πεδίου. Οι database columns
** χρησιμοποιούνται από το πρόγραμμα για να δώσουν data types σε
** διάφορες μεταβλητές. Η απόδοση των data types δεν γίνεται όμως από
** το παρόν πρόγραμμα αλλά θα γίνει αργότερα μέσα από το παραγόμενο
** awk script, καθώς δεν επιθυμώ εδώ να ανοίξω την database.
*/

typedef struct DB_COLUMN {
	DB_TABLE *table;	/* database table's pointer */
	char *column;		/* database column's name */
	struct DB_COLUMN *next;	/* next database column in the list */
} DB_COLUMN;

/*
** Η structure `VARIABLE' χρησιμοποιείται για τις μεταβλητές που
** εμφανίζονται στα awkrpt scripts. Μεταβλητές θεωρούνται όλα τα
** πεδία του input list και τα πεδία του sort list. Κάθε μεταβλητή
** πρέπει να έχει συγκεκριμένο τύπο που καθορίζεται είτε ρητά
** (string, numeric, date, float, time), είτε έμμεσα, ως database
** column. Οι τύποι έχουν τις παρακάτω αριθμητικές τιμές:
**
** 0: (null)	(unknown data type, use database column)
** 1: numeric	(ακέραιος αριθμός)
** 2: string	(αλφαριθμητικό)
** 3: date	(ημερομηνία)
** 4: float	(δεκαδικός αριθμός)
** 5: time	(ώρα)
*/

#define TYPE_NUMERIC 1
#define TYPE_STRING 2
#define TYPE_DATE 3
#define TYPE_FLOAT 4
#define TYPE_TIME 5

typedef struct VARIABLE {
	char *name;		/* variable's name */
	int input;		/* θέση στο input (1-base) */
	int sort;		/* θέση στο sort (1-base) */
	int type;		/* variable's data type */
	DB_COLUMN *column;	/* database column for zero data type */
	struct VARIABLE *next;	/* next variable in the list */
} VARIABLE;

/*
** Η structure `PARAMETER' χρησιμοποιείται στη διαχείριση των
** section parameters. Ως section parameters λογίζονται οι local
** variables των διαφόρων sections. Πράγματι, κάθε ένα από τα
** διάφορα sections καταγράφεται ως ξεχωριστή awk function στο
** παραγόμενο (object) awk script. Στις functions αυτές μπορούμε
** να χρησιμοποιήσουμε local variables παρεμβάλλοντας πριν από
** των awk κώδικα μια λίστα της μορφής (p1, p2, ..., pn) όπου
** p1, p2,..., pn είναι ονόματα awk μεταβλητών. Αυτή η λίστα
** θα περάσει ως local variable list στην παραγόμενη function.
*/

typedef struct PARAMETER {
	char *name;		/* parameter's name */
	struct PARAMETER *next;	/* next list parameter */
} PARAMETER;

#define ORDER_UDF (-1)		/* undefined order type */
#define ORDER_ASC 1		/* ascending order */
#define ORDER_DSC 2		/* descending order */
#define ORDER_NOT 3		/* don't sort */

typedef struct SORT {
	VARIABLE *var;		/* variable to sort */
	int ord;		/* 0 = descending, else ascending */
	int input;		/* θέση στο sorted input */
	TEXT *before;		/* awk code to run before change */
	PARAMETER *bplist;	/* `before' parameters' list */
	TEXT *after;		/* awk code to run after change */
	PARAMETER *aplist;	/* `after' parameters' list */
	struct SORT *next;
	struct SORT *prev;
} SORT;

#define AACS_ARCH 1
#define AACS_APPL 2
#define AACS_CMD 3
#define AACS_SCRIPT 4
#define AACS_OPT 5

typedef struct ARCHIVE {
	char *name;		/* awk archive file name */
	int type;		/* archive/application/command/script */
	struct ARCHIVE *next;
} ARCHIVE;

#define STR_NULL ((char *)(NULL))
#define TXT_NULL ((TEXT *)(NULL))
#define COL_NULL ((DB_COLUMN *)(NULL))
#define TBL_NULL ((DB_TABLE *)(NULL))
#define VAR_NULL ((VARIABLE *)(NULL))
#define PRM_NULL ((PARAMETER *)(NULL))
#define SRT_NULL ((SORT *)(NULL))
#define ARC_NULL ((ARCHIVE *)(NULL))

extern char *source;			/* source file name */
extern int lineno;			/* source input line number */
extern int n_errs;			/* syntax error counter */
extern DB_TABLE *tbl_list;		/* database tables' list */
extern DB_COLUMN *col_list;		/* database columns' list */
extern VARIABLE *var_list;		/* variables' list */

extern TEXT *input_code;
extern TEXT *begin_code;
extern TEXT *before_report_code;
extern TEXT *before_first_data_code;
extern TEXT *before_accepted_data_code;
extern TEXT *before_data_code;
extern TEXT *detail_code;
extern TEXT *after_sort_code;
extern TEXT *after_data_code;
extern TEXT *after_last_data_code;
extern TEXT *after_report_code;
extern TEXT *after_accepted_data_code;
extern TEXT *end_code;
extern TEXT *functions_code;

extern PARAMETER *input_plist;
extern PARAMETER *begin_plist;
extern PARAMETER *before_report_plist;
extern PARAMETER *before_first_data_plist;
extern PARAMETER *before_accepted_data_plist;
extern PARAMETER *before_data_plist;
extern PARAMETER *detail_plist;
extern PARAMETER *after_sort_plist;
extern PARAMETER *after_data_plist;
extern PARAMETER *after_last_data_plist;
extern PARAMETER *after_report_plist;
extern PARAMETER *after_accepted_data_plist;
extern PARAMETER *end_plist;
extern PARAMETER *functions_plist;

extern VARIABLE *input_list;		/* input items' list */
extern SORT *sort_list;			/* sort items' list */
extern SORT *sort_last;			/* sort list's last item */
extern int aacs_type;			/* archive, application or */
					/* command line string */
extern ARCHIVE *archive_list;		/* AWK archives' list */
extern ARCHIVE *application_list;	/* AWK applications' list */
extern int n_input;			/* input fields' count */
extern int n_sort;			/* sort fields' count */
extern int sorted;			/* 1 if sorted input, else 0. */
					/* default -1 means undefined */
extern int tpm_format;			/* tpm post processing */
extern int readable_object;		/* object human readable */
extern char *post_program;		/* post processor command */
					/* line */
extern char *ante_program;		/* pre processor command line */
extern char ante_option;		/* -A input generator, or -a */
extern int remove_script;		/* remove script after report */
extern char *script_name;		/* awkrpt source script name */
extern char *object_name;		/* awkrpt object script name */
extern int plist_on;			/* parameters' list flag */

extern int main(int, char *[]);
extern void warning(char *s);
extern int yyerror(char *);
extern void fatal(char *s, const int);
extern TEXT *txt_add(TEXT *, char *, int);
extern TEXT *txt_more(TEXT *, TEXT *);
extern void txt_print(TEXT *, FILE *, int);
extern VARIABLE *var_add(char *, int);
extern void review_variables(void);
extern void review_sort_fields(void);
extern void table_columns(void);
extern DB_COLUMN *col_add(char *, int);
extern void object(int, int, char *, int);
extern void generate(void);
extern void add_archive(char *, int);
extern void print_archives(void);
extern SORT *sort_push(VARIABLE *, int);
extern char *post_ante(char *, char *);
extern int exec_report(int, char *[], int);
extern int interpret(int, char *[]);
extern void usage(void);
extern void cs_print(char *);
extern void set_progname(char *s);
