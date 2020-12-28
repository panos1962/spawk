#include "sparc.h"
#include <unistd.h>

#define MODE_REPORT 0
#define MODE_COMPILE 1
#define MODE_INTERPRET 2

// Program runs either as a compiler, or as an interpreter, or as a report
// processor.
//
// Given the -c option the program runs as a compiler. In this case we can
// supply the -b, -o and -O options. Option -b produces readable AWK code,
// while option -o prints the AWK script to the standard output, instead of
// producing a report object (.ro) script.
//
// Compiler mode
// -------------
// In compiler mode, if a single source is given a single report object (.ro)
// script will be produced, e.g.
//
//	sparc -c test1
//
// will produce AWK code to "test1.ro" file, while:
//
//	sparc test1.1 test1.2 test1.3
//
// will produce AWK code to the standard output.
//
// When in compiler mode and a single file is given, then produce a report
// object (.ro) file. If more than one files given, then the report object
// code will be printed to standard output. If the option -O is given then
// the report object code will be printed to the standard output even if a
// single source file is given, e.g.
//
//	sparc -c -O test1
//
// will print report object code to the standard output.
//
// Option -o takes an argument which is the report opbject (.ro) file name,
// e.g. both of the
//
//	sparc -c -o panos test1
//
// and
//
//	sparc -c -o panos test1.1 test1.2 tyest1.3
//
// commands will produce the "panos.ro" report object script.

/*
** Το πρόγραμμα μπορεί να τρέξει και ως interpreter με χρήση της
** option -s ακολουθούμενης από το source script, π.χ.
**
**     sparc -s test1 data1 data2 - data3
**
** τρέχει το script test1 διαβάζοντας input από τα files data1, data2,
** από το standard input και τέλος από το data3. Σ'αυτήν την περίπτωση
** το πρόγραμμα κάνει on-the-fly compilation παράγοντας AWK κώδικα σε
** προσωρινό file (στο /tmp directory) και κατόπιν εκτελεί το παραχθέν
** script το οποίο διαγράφεται μετά το τέλος του report processing.
**
** Χωρίς τις options -c και -s το πρόγραμμα τρέχει απυθείας σε report
** processing mode και ως πρώτο command line argument δίνεται το AWK
** script που παρήχθη σε προγενέστερη φάση από compilation. Π.χ., αν
** έχω το script test1 ακολουθώ τα εξής βήματα:
**
** 1. Παράγω το object με compilation:
**
**     sparc -c test1
**
** 2. Παράγω reports χρησιμοποιώντας το object που παρήχθη, π.χ.
**
**     sparc test1.ro data1 data2
**
** Σε report processing mode παρέχεται και η option -r με την οποία
** διαγραφή του (object) script μετά την εκτέλεση του report. Αυτή
** η option δεν μπορεί να συνδυαστεί με την -s, δηλαδή δεν μπορώ να
** δηλώσω διαγραφή του (object) script εφόσον δουλεύω με το source
** script.
**
** Τέλος, παρέχεται η dummy option -R που συνοδεύεται από ένα αυθαίρετο
** string (report pipe id). Η option μαζί με το pipe-id αγνοούνται αλλά
** μπορεί να φανεί χρήσιμη σε εσωτερικά awk pipes όπου έχουμε χειρισμό
** των pipes σύμφωνα με τα αντίστοιχα command lines, πράγμα που ίσως
** μας δυσκολέψει σε περιπτώσεις που τρέχουμε παράλληλα το ίδιο
** subreport περισσότερες από μία φορές· σ'αυτές τις περιπτώσεις
** διαφοροποιούμε τα subreports δίνοντας την option -R με διαφορετικά
** ids, π.χ.
**
**     ...
**     rpt_param1 = "sparc -R 1 param.ro"
**     rpt_param2 = "sparc -R 2 param.ro"
**     ...
*/

int main(int argc, char *argv[]) {
	int c;
	int errs = 0;
	extern int optind;
	extern char *optarg;
	int source_count = 0;
	int create_object = 1;
	int mode = MODE_REPORT;
	char *report_id = STR_NULL;
	int no_post_ante = 0;
	int opt_offset = 3;
	extern int yyparse(void);

	argv[0] = cht_set_progname(argv[0]);
	while ((c = getopt(argc, argv, ":R:rXco:ObsS")) != EOF) {
		switch (c) {
		case 'c':	/* compile source scripts */
			mode = MODE_COMPILE;
			break;
		case 'o':	/* object name given */
			object_name = optarg;
			break;
		case 'O':	/* print report object code to stdout */
			create_object = 0;
			break;
		case 'b':	/* produce readable object (beautyfier) */
			readable_object = 0;
			break;
		case 'R':	/* report pipe id */
			if (optind != opt_offset) {
				cht_error("-R must be the first option given");
				errs++;
			}

			else
			report_id = optarg;

			break;
		case 's':	/* compile and run report */
			mode = MODE_INTERPRET;
			remove_script = 0;
			break;
		case 'r':	/* remove object script after report */
			mode = MODE_REPORT;
			remove_script = 1;
			break;
		case 'X':
			no_post_ante = 1;
			opt_offset++;
			break;
		case 'S':
			sorted = 1;
			break;
		case '?':
			errs += cht_erropt();
			break;
		}
	}

	if (errs)
	usage();

	argc -= optind;
	argv += optind;

	switch (mode) {

	case MODE_REPORT:
	exit(exec_report(argc, argv, no_post_ante));

	case MODE_INTERPRET:
	exit(interpret(argc, argv));

	}

	/*
	** Έχει δοθεί η option -c και είμαστε σε compilation
	** mode. Σ'αυτήν την περίπτωση διατρέχουμε τα scripts
	** και κάνουμε συντακτικό έλεγχο. Αν δεν παρουσιαστούν
	** σφάλματα προχωρούμε σε παραγωγή AWK κώδικα (object)
	** ο ποίος είτε θα γραφεί σε `.ro' file, είτε θα τυπωθεί
	** στο standard output. Ο κώδικας που παράγεται είναι
	** μεν AWK κώδικας, αλλά περιέχει και awkrpt directives
	** σχετικά με τις AWK βιβλιοθήκες που πρέπει να ληφθούν
	** υπόψη, καθώς επίσης και directives που αφορούν στο
	** pre/post processing. Αυτά τα directives είναι γραμμές
	** που ξεκινούν με `#@' που είναι μεν σχόλια για τον AWK,
	** αλλά ο awkrpt τα μετατρέπει σε κατάλληλες chtawk options
	** και καλεί τον chtawk με το ζητούμενο command line.
	*/

	if (report_id != STR_NULL)
	usage();

	if (sorted > 0)
	cht_error("WARNING: input considered sorted");

	if (!argc)
	yyparse();

	else for (; argc > 0; argc--, argv++) {
		source = *argv;
		lineno = 1;
		source_count++;
		if (freopen(source, "r", stdin) == NULL) {
			cht_error(STR_NULL);
			perror(source);
			n_errs++;
		}
		else
			yyparse();
	}

	if (n_errs > 0) {
		cht_error("errors encountered (compilation failed)");
		exit(EXIT_SYNTAX);
	}

	review_variables();
	review_sort_fields();
	object(create_object, source_count, source, readable_object);
	print_archives();
	generate();
	exit(EXIT_OK);
}
