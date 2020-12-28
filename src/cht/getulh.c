#include <stdlib.h>
#include <stdio.h>
#include <cht.h>
#include <utmpx.h>
#define UTMP struct utmpx
#define UNULL ((UTMP *)0)

char *cht_getulh(const int ulh)
{
	static UTMP *u;
	register int idx;

	if (u != UNULL)
		goto UTMPX_OK;

	if ((idx = ttyslot() + 1) <= 0) {
		cht_error(NULL);
		perror("getulh: ttyslot");
		goto ERROR;
	}

	while (idx--) {
		if ((u = getutxent()) == UNULL) {
			cht_error(NULL);
			perror("getulh: getuent");
			goto ERROR;
		}
	}

UTMPX_OK:
	switch (ulh) {
	case CHT_ULH_USER:
		return u->ut_user;
	case CHT_ULH_LINE:
		return u->ut_line;
	case CHT_ULH_HOST:
		return u->ut_host;
	}

	cht_error(NULL);
	fprintf(stderr, "%d: invalid ulh\n", ulh);

ERROR:
	return "";
}
