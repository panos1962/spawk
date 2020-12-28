#include <string.h>
#include <ctype.h>
#include <cht.h>

#define MAX_N_VAR 100
#define SNULL ((char *)(NULL))
#define LNULL ((char **)(NULL))

static int varcmp(register char *s, register char *p)
{
	while (*s) {
		if (*p == '=')
			return 0;

		if (!(*p))
			return 0;

		if (*s++ != *p++)
			return 0;
	}

	return (*p == '=');
}

int cht_putenv(char *var, char *val)
{
	int i;
	char *p;
	static char **vlist = LNULL;
	static int n_var = 0;
	char *func_name = "cht_putenv";

	if (vlist == LNULL) {
		if ((vlist = (char **)calloc(MAX_N_VAR,
			sizeof(char *))) == LNULL) {
			cht_error(NULL);
			fprintf(stderr, "%s: out of memory\n",
				func_name);
			return 1;
		}

		for (i = 0; i < MAX_N_VAR; i++)
			vlist[i] = SNULL;
	}

	if (var == SNULL) {
		for (i = 0; i < n_var; i++) {
			if (vlist[i] != SNULL)
				free(vlist[i]);
		}

		free(vlist);
		vlist = LNULL;
		return 0;
	}

	i = 0;			/* used here as error flag */
	p = var;
	if ((*p == 0) || ((!isalpha(*p)) && (*p != '_')))
		i = 1;
	else {
		for (++p; *p; p++) {
			if (isalnum(*p))
				continue;

			if (*p != '_')
				i = 1;
		}
	}

	if (i) {
		cht_error(NULL);
		fprintf(stderr, "%s: %s: invalid variable name\n",
			var, func_name);
		return 1;
	}

	if (val == SNULL)
		val = "";

	if ((p = (char *)malloc(strlen(var) +
		strlen(val) + 2)) == SNULL) {
		cht_error(NULL);
		fprintf(stderr, "%s: out of memory\n", func_name);
		return 1;
	}

	sprintf(p, "%s=%s", var, val);
	if (putenv(p)) {
		cht_error(NULL);
		perror(p);
		free(p);
		return 1;
	}

	for (i = 0; i < n_var; i++) {
		if (varcmp(var, vlist[i])) {
			free(vlist[i]);
			vlist[i] = p;
			return 0;
		}
	}

	if (n_var >= MAX_N_VAR) {
		cht_error(NULL);
		fprintf(stderr, "%s: too many environment variables\n",
			func_name);
		free(p);
		return 1;
	}

	vlist[n_var++] = p;
	return 0;
}
