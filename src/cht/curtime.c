/*
** Η function αυτή ετοιμάζει την ημερομηνία/ώρα
** στο array mdyhms, που είναι έξι θέσεων με format
** mounth, day, year, hour, minute, second.
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <cht.h>

void cht_curtime(int mdyhms[])
{
	time_t cl;
	struct tm *p;

	time(&cl);
	p = localtime(&cl);
	mdyhms[0] = p->tm_mon + 1;
	mdyhms[1] = p->tm_mday;
	mdyhms[2] = (int)1900 + p->tm_year;
	mdyhms[3] = p->tm_hour;
	mdyhms[4] = p->tm_min;
	mdyhms[5] = p->tm_sec;
}
