#include "sparc.h"
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char *cht_progname = "sparc";

char *cht_set_progname(char *s) {
	if (!s)
	return cht_progname;

	if (!(cht_progname = strrchr(s, '/')))
	cht_progname = s;

	else
	cht_progname++;

	return cht_progname;
}

FILE *cht_sfreopen(char *path, char *mode, FILE *fp, int err) {
	if (freopen(path, mode, fp))
	return fp;

	cht_error(NULL);
	perror(path);
	exit(err);
}

char *cht_strsave(char *s, int err) {
	char *p;
	int l;

	if (!s)
	return s;

	l = strlen(s) + 1;
	if (!(p = malloc(l)))
	cht_fatal("strsave: out of memory", err);

	memcpy(p, s, l);
	return p;
}

char *cht_strnsave(char *s, int l, int err) {
	char *p;

	if (!(p = malloc(l + 1)))
	cht_fatal("strnsave: out of memory", err);

	memcpy(p, s, l);
	*(p + l) = 0;
	return p;
}

// A "smart" strcpy.
// Returns the end of the string instead of the beginning.
// Can be called in sequence to concatenate strings.

char *cht_strcpy(char *t, char *s) {
	while (*s)
	*t++ = *s++;

	*t = 0;
	return t;
}

static void filter_wait(void) {
	fflush(stdout);
	fclose(stdout);

	while (wait((int *)0) != -1);
}

void cht_filter(char *prog, int err) {
	FILE *fp;
	static int first_time = 1;

	if (!(fp = popen(prog, "w")))
	goto ERROR;

	if (close(1) == -1)		/* close stdout */
	goto ERROR;

	if (dup2(fileno(fp), 1) == -1)	/* make stdout equals fp */
	goto ERROR;

	if (close(fileno(fp)) == -1)	/* pipe is now stdout */
	goto ERROR;

	if (first_time) {
		atexit(filter_wait);
		first_time = 0;
	}

	return;

ERROR:
	cht_error(NULL);
	fputs("cht_filter: ", stderr);
	perror(prog);
	exit(err);
}

void cht_error(char *s) {
	if (!cht_progname) {
		fputs(cht_progname, stderr);
		fputs(": ", stderr);
	}

	if (s) {
		fputs(s, stderr);
		putc('\n', stderr);
	}
}

void cht_fatal(char *s, int err) {
	if (s == NULL)
	s = "fatal error";

	cht_error(s);
	exit(err);
}

int cht_erropt(void) {
	extern int optopt;

	cht_error(NULL);
	fprintf(stderr, "illegal option -- %c\n",
		(isprint(optopt) ? optopt : '?'));
	return 1;
}

void cht_nomem(char *msg, int err) {
	cht_error(NULL);
	if (msg) {
		fputs(msg, stderr);
		fputs(": ", stderr);
	}

	fputs("out of memory\n", stderr);
	exit(err);
}
