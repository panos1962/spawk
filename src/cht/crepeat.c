/*
** Η function τυπώνει στον file pointer fp τον χαρακτήρα c n φορές.
*/

#include <stdio.h>
#include <stdlib.h>
#include <cht.h>

void cht_crepeat(char c, FILE *fp, register int n)
{
	while (n--)
		putc(c, fp);
}
