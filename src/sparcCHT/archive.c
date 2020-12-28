#include "sparc.h"
#include <string.h>

/*
** Η function `add_archive' καλείται για να προσθέσουμε ένα archive
** στο archive list (`archive_list'). Το archive list είναι LIFO
** και περιέχει τα ονόματα των AWK libraries που θέλουμε να μετέχουν
** στο report. Ως γνωστόν, τα archives δηλώνονται είτε με τα keywords
** `archive' ή `archives' όπου δίνουμε απευθείας τα archive file names,
** είτε με τα keywords `applications' ή `application' όπου δίνουμε
** τα ονόματα των εφαρμογών (π.χ. prosop, adia κλπ) και εννοούμε το
** αντίστοιχο archive π.χ. για την εφαρμογή prosop εννοούμε το archive
** /cht/apps/prosop/lib/awklib. Η function ελέγχει και απορρίπτει τις
** διπλοεγγραφές (με όποιο τρόπο και αν έχουν δοθεί).
*/

void add_archive(char *fname, int ftype)
{
	char *p;
	int l;
	ARCHIVE *a;
	char buf[MAX_FILE_NAME_LENGTH + 96];
	char *pck;

	if (fname == STR_NULL)
		return;

	if (strlen(fname) > MAX_FILE_NAME_LENGTH) {
		yyerror(STR_NULL);
		fprintf(stderr, "%s: huge archive/application name\n",
			fname);
		free(fname);
		return;
	}

	switch (ftype) {
	case AACS_APPL:
		sprintf(buf, "/cht/apps/%s/lib/awklib", fname);
		pck = buf;
		break;
	case AACS_ARCH:
		pck = fname;
		break;
	case AACS_OPT:
		goto ADD_AACS;
	default:
		yyerror(STR_NULL);
		fprintf(stderr, "%s: undefined archive/application "
			"mode\n", fname);
		free(fname);
		return;
	}

	for (a = archive_list; a != ARC_NULL; a = a->next) {
		if (!strcmp(a->name, pck)) {
			warning(STR_NULL);
			fprintf(stderr, "%s: redefined archive "
				"(ignored)\n", fname);
			free(fname);
			return;
		}
	}

ADD_AACS:
	if ((void *)((a = (ARCHIVE *)malloc(sizeof(ARCHIVE)))) == NULL)
		cht_nomem("add_achive (node)", EXIT_MEMORY);

	a->name = fname;
	a->type = ftype;
	a->next = archive_list;
	archive_list = a;
}

static char *directive(void)
{
	return(readable_object ? "%" : "#@");
}

static void print_command(char *cmd, int pa)
{
	char *p;

	if (cmd == STR_NULL)
		return;

	printf("%s -%c \"", directive(), pa);
	cs_print(cmd);
	putchar('"');
	putchar('\n');
}

/*
** Η function `print_archives' καλείται κατά την αρχή της παραγωγής
** του object script. Τυπώνει μια επικεφαλίδα και όλα τα `chtawkrpt'
** directives που αφορούν σε AWK libraries και pre/post processing.
** Τα directives του chtawkrpt είναι γραμμές που ξεκινούν με το string
** `#@' και ακολουθούνται από ένα κενό και τις `chtawk' options που
** επιθυμούμε για το παραγόμενο awk script. Με ένα απλό directive
** χωρίς επιπλέον arguments δηλώνουμε το τέλος των `chtawkrpt'
** directives.
*/

void print_archives(void)
{
	ARCHIVE *a;

	printf("#!/bin/awk\n\n");
	for (a = archive_list; a != ARC_NULL; a = a->next) {
		printf("%s ", directive());
		switch (a->type) {
		case AACS_ARCH:
			printf("-f %s\n", a->name);
			break;
		case AACS_APPL:
			printf("-S %s\n", a->name);
			break;
		case AACS_OPT:
			printf("%s\n", a->name);
			break;
		}
	}

	if (tpm_format)
		printf("%s -t\n", directive());

	print_command(ante_program, ante_option);
	print_command(post_program, 'p');
	printf("%s\n", directive());
}

/*
** Η function `cs_print' χρησιμοποιείται για την εκτύπωση των
** ante/post command lines, όπως επίσης και για την εκτύπωση του
** script name. Και στις δύο αυτές περιπτώσεις θα δημιουργηθεί
** πρόβλημα αν στα αντίστοιχα strings περιέχονται διπλά quotes (").
** Η function τυπώνει τα εν επίμαχα strings παρεμβάλλοντας backslash
** πριν από τα διπλά quotes (αν υπάρχουν).
*/

void cs_print(register char *s)
{
	if (s == STR_NULL)
		return;

	for (; *s; s++) {
		if (*s == '"')
			putchar('\\');

		putchar(*s);
	}
}
