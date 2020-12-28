#include "sparc.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

static char *add_command(char *cmd, char *arg) {
	char *p;
	static int len = 0;

	*cmd++ = ' ';
	len++;
	for (p = arg; *p; p++) {
		if (*p == '\n')
			break;

		*cmd++ = *p;
		len++;
		if (len >= MAX_COMMAND_LENGTH)
			cht_fatal("huge report command", EXIT_REPORT);
	}

	return(cmd);
}

static FILE *try_this_file(char *fname) {
	char *p;
	FILE *fp;

	for (p = fname; *p; p++)
		;

	if (((p - 3) < fname) || strcmp(p - 3, ".ro")) {
		strcpy(p, ".ro");
		if ((fp = fopen(fname, "r")) != (FILE *)(NULL))
			return(fp);

		*p = 0;
	}

	return(fopen(fname, "r"));
}

static FILE *open_script(char **script) {
	static char *path;
	static char *end;
	char *p;
	char *q;
	static char pathname[MAX_FILE_NAME_LENGTH + 96];
	char *t;
	char *s;
	FILE *fp;
	struct stat st;

	if ((**script == '/') ||
		((path = getenv("CHT_AWKRPTPATH")) == STR_NULL))
		path = "";

	for (end = (path = cht_strsave(path, EXIT_MEMORY)); *end; end++) {
		if (*end == ':')
			*end++ = 0;
	}

	if (end <= path)
		end++;

	for (p = path; p < end; p = q + 1) {
		for (t = pathname, q = p; *q; q++) {
			if (t - pathname > MAX_FILE_NAME_LENGTH)
			goto ERROR;

			*t++ = *q;
		}

		if (t > pathname)
		*t++ = '/';

		for (s = *script; *s; s++) {
			if (t - pathname > MAX_FILE_NAME_LENGTH)
			goto ERROR;

			*t++ = *s;
		}

		*t = 0;
		if (stat(pathname, &st)) {
			strcpy(t, ".ro");
			if ((fp = fopen(pathname, "r")) != (FILE *)NULL) {
				*script = pathname;
				return(fp);
			}

			*t = 0;
		}

		if (S_ISDIR(st.st_mode))
		strcpy(t, "/main");

		else if (!(S_ISREG(st.st_mode)))
		continue;

		if ((fp = try_this_file(pathname)) != (FILE *)(NULL)) {
			*script = pathname;
			return(fp);
		}
	}

	errno = ENOENT;
	return (FILE *)(NULL);

ERROR:
	cht_fatal("huge script name", EXIT_FATAL);
}

/*
** Είμαστε σε report processing mode (runtime) και πρέπει να έχουμε
** πρώτο command line argument το όνομα του (object) script. Ακολουθούν
** (προαιρετικά) τα data files. Αν δεν καθοριστούν data files, τότε
** το πρόγραμμα διαβάζει data από το standard input. Το πρόγραμμα θα
** διαβάσει το (object) script και θα μετατρέψει τα `#@' directives
** στις κατάλληλες `chtawk' options. Αφού κατασκευάσει το chtawk command
** line το εκτελεί παράγοντας τελικά το report.
*/

int exec_report(int argc, char *argv[], int npa) {
	char *script;
	FILE *fp;
	char buf[MAX_FILE_NAME_LENGTH + 96];
	char cmd[MAX_COMMAND_LENGTH + 196];
	char *pcmd;

	if (argc <= 0) {
		sprintf(cmd, "exec cat");
		goto EXEC_REPORT;
	}

	set_progname(script = *argv);
	if ((fp = open_script(&script)) == (FILE *)(NULL)) {
		cht_error(STR_NULL);
		perror(script);
		exit(EXIT_REPORT);
	}
	
	/*
	** Ξεκινάει η κατασκευή του `awk' command line με πρώτη
	** την -R option που θα προκαλέσει συμπερίληψη της `rpt'
	** AWK library (/cht/lib/awk/chtrpt.awk).
	*/

	pcmd = cht_strcpy(cmd, "exec awk -f sparc.awk");

	/*
	** Διαβάζουμε τώρα το (object) `chtawkrpt' script το οποίο
	** ίσως περιέχει `#@' directives. Αυτά τα directives είναι
	** `chtawk' options που προστίθενται στο παραγόμενο `chtawk'
	** command line. Για efficiency reasons φροντίζουμε κατά
	** την παραγωγή του object κώδικα να τελειώνουμε το τμήμα
	** των directives με ένα ασυνόδευτο directive (#@), ώστε να
	** μη χρειάζεται τώρα να διατρέχουμε τώρα όλο το object script.
	*/

	while (fgets(buf, MAX_FILE_NAME_LENGTH, fp) != NULL) {
		if (strncmp(buf, "#@", 2))
		continue;

		if (buf[2] == '\n')	/* end directives */
		break;

		pcmd = add_command(pcmd, buf + 3);
	}

	fclose(fp);

	if (npa)
	pcmd = add_command(pcmd, "-X");

	if (sorted > 0)
	pcmd = add_command(pcmd, "-n cht_rpt_sorted=1");

	/*
	** Έχουμε διαβάσει τα directives που ίσως υπήρχαν στο
	** object script και μένει η συμπερίληψη του ίδιου του
	** object (AWK) script. Η συμπερίληψη γίνεται είτε με την
	** option -f, είτε με την option -r εφόσον έχει δοθεί
	** η -r option που σημαίνει διαγραφή του object script
	** μετά την εκτέλεση του report.
	*/

	*pcmd++ = ' ';
	*pcmd++ = '-';
	*pcmd++ = (remove_script ? 'r' : 'f');
	pcmd = add_command(pcmd, script);

	/*
	** Τέλος, προστίθενται στο παραγόμενο `chtawk' command line
	** τα data files ή όποια άλλα command line arguments έχουν
	** δοθεί.
	*/

	for (argc--, argv++; argc > 0; argc--, argv++)
	pcmd = add_command(pcmd, *argv);

	*pcmd = 0;

EXEC_REPORT:
	execlp("/bin/sh", "sh", "-c", cmd, STR_NULL);
	exit(EXIT_FATAL);
}
