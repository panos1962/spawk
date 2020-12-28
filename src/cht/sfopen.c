/*
** Η cht_sfopen ανοίγει ένα file και αν αποτύχει,
** τυπώνει μήνυμα λάθους και κάνει exit.
*/

#include <stdlib.h>
#include <stdio.h>
#include <cht.h>

FILE *cht_sfopen(char *path, char *mode, int err)
{
	FILE *fp;

	if ((void *)(fp = fopen(path, mode)) == NULL) {
		cht_error(NULL);
		perror(path);
		exit(err);
	}

	return fp;
}
