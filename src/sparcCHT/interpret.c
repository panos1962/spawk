#include "sparc.h"
#include <string.h>
#include <unistd.h>

/*
** Η function εκτελεί το report σε interpreter mode. Εδώ δίνεται ως
** πρώτο command line argument το όνομα του source script και, αφού
** δημιουργηθεί temporary object script (compilation), ξανακαλείται
** το πρόγραμμα σε report processing mode με το παραχθέν object script
** και με όλες τις υπόλοιπες options και άλλα command lines arguments
** που είχε κατά την αρχική κλήση.
*/

int interpret(int argc, char *argv[])
{
	char buf[64];
	char cmd[MAX_COMMAND_LENGTH + 196];
	char **argv2;
	int fd;

	if (argc <= 0)
		usage();

	if ((fd = mkstemp(strcpy(buf, "/tmp/ro.XXXXXX"))) < 0) {
		cht_error(STR_NULL);
		perror(buf);
		exit(EXIT_FATAL);
	}

	/*
	** Κλείνω το file descriptor που αφορά στο temporary file
	** name που πήρα μόλις πριν με την `mkstemp', και κάνω
	** compilation στο source script παράγοντας object κώδικα
	** στο temporary file.
	*/

	close(fd);
	set_progname(argv[0]);
	sprintf(cmd, "%s -c%co %s %s", cht_progname, 
		(sorted > 0 ? 'S' : 'c'), buf, argv[0]);
	if (system(cmd))
		exit(EXIT_SYNTAX);

	/*
	** Εδώ επαναδιαμορφώνω τα command line arguments και τρέχω
	** τελικά το report με πρώτη την -r option ώστε το temporary
	** object που θέτω ως δεύτερο command line argument να διαγραφεί
	** μετά την εκτέλεση του report.
	*/

	if ((argv2 = calloc(argc + 4, sizeof(*argv2))) == NULL)
		cht_nomem("exec_art", EXIT_MEMORY);

	argv2[0] = cht_progname;
	argv2[1] = "-r";
	argv2[2] = buf;
	for (fd = 1; fd < argc; fd++)
		argv2[fd + 2] = argv[fd];

	argv2[fd + 2] = NULL;
	execvp(cht_progname, argv2);
	cht_fatal("interpretive execution failed", EXIT_FATAL);
}
