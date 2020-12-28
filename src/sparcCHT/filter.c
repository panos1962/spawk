#include "sparc.h"
#include <string.h>

/*
** Με τα keywords `post' και `ante' παρεμβαίνουμε στο output και στο
** input αντίστοιχα του chtawktpt. Πιο συγκεκριμένα, αν δώσουμε το
** keyword `post' συνοδευόμενο από command line (μέσα σε απλά quotes)
** το command line που δίνουμε θα τρέξει ως post-processor στο
** παραγόμενο output· αν δώσουμε το keyword `ante' συνοδευόμενο από
** command line (μέσα σε απλά quotes), θα έχω input pre-processing
** στο awkrpt input stream. Μπορώ να καθορίσω ολόκληρα pipelines ως
** post/pre processors αλλά μπορώ επίσης να δηλώσω επαναληπτικά
** post/pre processors οπότε το πρόγραμμα τα συνδέει σε pipes.
*/

char *post_ante(char *cmd, char *add)
{
	char buf[MAX_FILE_NAME_LENGTH + 96];

	if ((add == STR_NULL) || ((cmd != post_program) &&
		(cmd != ante_program)))
		fatal("post_ante: internal error", EXIT_FATAL);

	if (cmd == STR_NULL)
		return(add);

	if (strlen(cmd) + strlen(add) > MAX_FILE_NAME_LENGTH)
		fatal("huge command line", EXIT_FATAL);

	sprintf(buf, "%s | %s", cmd, add);
	free(cmd);
	free(add);
	return(cht_strsave(buf, EXIT_MEMORY));
}
