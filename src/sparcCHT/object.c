#include "sparc.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>


#define MAX_FILE_NAME 1014

void object(int create_object, int source_count, char *source, int readable_object) {
	char obj_name[MAX_FILE_NAME + 10];
	register char *p;
	register char *q;

	if (!create_object)
	goto OBJECT_OK;

	if (object_name != STR_NULL)
	goto CREATE_OBJECT;

 	if ((source_count != 1) || (source == STR_NULL))
	goto OBJECT_OK;

	for (p = source, q = obj_name; *p; p++) {
		if (q - obj_name > MAX_FILE_NAME)
		cht_fatal("huge source name", EXIT_FATAL);

		*q++ = *p;
	}

	if ((p - source > 2) && (!strncmp(p - 3, ".rs", 3)))
	q -= 3;

	*q++ = '.';
	*q++ = 'r';
	*q++ = 'o';
	*q = 0;
	object_name = obj_name;

CREATE_OBJECT:
	umask(~0664);
	cht_sfreopen(object_name, "w", stdout, EXIT_OBJECT);

fprintf(stderr, "readable_object: %d\n", readable_object);
OBJECT_OK:
	if (readable_object)
	cht_filter("spawk -CC", EXIT_OBJECT);
}
