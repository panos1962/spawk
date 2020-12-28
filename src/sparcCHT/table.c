#include "sparc.h"

/*
** Η function `table_columns' παράγει awk κώδικα σχετικό με την
** εισαγωγή των data types των πεδίων των πινάκων της database
** που εμφανίζονται στο awkrpt script είτε σε ονόματα μεταβλητών
** είτε ως data types. Αυτό το τμήμα του awk κώδικα εντάσσεται
** συνήθως στον before first data παραγόμενο κώδικα καθώς φαίνεται
** ότι δεν έχει νόημα να κληθεί εφόσον δεν υπάρχει input.
*/

void table_columns(void)
{
	DB_TABLE *t;

	if (tbl_list == TBL_NULL)
		return;

	printf("\n\t## Εδώ φορτώνονται οι τύποι πεδίων που θα μας\n"
		"\t## χρειαστούν από πίνακες της database.\n");
	for (t = tbl_list; t != TBL_NULL; t = t->next)
		printf("\tif (cht_table_columns(\"%s\", 1) == \"\")"
			" err++\n", t->name);
}
