/*
** Η function αυτή πρέπει να καλείται στην αρχή του προγράμματος
** και θέτει το όνομα του προγράμματος στην global μεταβλητή
** cht_progname. Σαν παράμετρο στη function ευτή περνάμε
** το argv[0] από τις παραμέτρους της main.
** Το όνομα αυτό χρησιμοποιείται κατόπιν μέσα από τις functions
** cht_error, cht_fatal κλπ, που τυπώνουν μηνύματα λάθους.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cht.h>

char *cht_set_progname(char *s)
{
	register char *p;

	if ((void *)(cht_progname = strrchr(s, '/')) == NULL)
		cht_progname = s;
	else
		cht_progname++;

	if (getenv("CHT_DEBUG") != (char *)(NULL))
		cht_debug_mode = 1;

	return cht_progname;
}
