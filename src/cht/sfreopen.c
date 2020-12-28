/*
** Η function cht_sfreopen κλείνει και ξανανοίγει κάποιον
** file pointer σε νέο file και αν αποτύχει, τυπώνει μήνυμα
** λάθους και κάνει exit.
*/

#include <stdlib.h>
#include <stdio.h>
#include <cht.h>

FILE *cht_sfreopen(char *path, char *mode, FILE *fp, int err)
{
	if ((void *)freopen(path, mode, fp) == NULL) {
		cht_error(NULL);
		perror(path);
		exit(err);
	}

	return fp;
}
