#include <cht.h>

void cht_tpm_data(int yes)
{
	if (yes)
		puts(".DM");
	else
		putchar('\034');
}
