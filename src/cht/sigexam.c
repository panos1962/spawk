#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <cht.h>

void cht_sigexam(char *msg)
{
	int sig;
	void (*psh)();

	if (msg != (char *)0) {
		fputs(msg, stderr);
		putc('\n', stderr);
	}

	for (sig = 1; sig < NSIG; sig++) {
		fprintf(stderr, "%2d: ", sig);
		if ((psh = signal(sig, SIG_IGN)) == SIG_ERR) {
			fputs("SIG_ERR\n", stderr);
			continue;
		}

		if (psh == SIG_DFL)
			fputs("SIG_DFL\n", stderr);
		else if (psh == SIG_IGN)
			fputs("SIG_IGN\n", stderr);
		else
			fprintf(stderr, "%p\n");

		if (signal(sig, psh) == SIG_ERR) {
			cht_error(NULL);
			fprintf(stderr, "cannot reset previous signal "
				"handler for signal (%d)\n", sig);
		}
	}

	fflush(stderr);
}
