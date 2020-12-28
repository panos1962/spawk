#ifndef _CHT_H
#define _CHT_H

#ifndef _STDLIB_H
#include <stdlib.h>
#endif

#ifndef _STDIO_H
#include <stdio.h>
#endif

#define CHT_CHTDEV_GID (1001)
#define cht_arralloc(p, n, sz, msg, stat) { \
	if ((p = calloc(n, sz)) == NULL) \
		cht_nomem(msg, stat); \
}

extern char *cht_progname;
extern int cht_debug_mode;
extern char *cht_valck_errmsg;

extern char *cht_set_progname(char *);
extern void cht_usage(char *, int);
extern void cht_error(char *);
extern void cht_fatal(char *, int);
extern int cht_erropt(void);
extern void cht_nomem(char *, int);
extern void cht_expire(int, int, int, int);
extern void cht_sigexam(char *);

#define CHT_ULH_USER 1
#define CHT_ULH_LINE 2
#define CHT_ULH_HOST 3
extern char *cht_getulh(const int);

extern short cht_device(void);
extern FILE *cht_sfopen(char *, char *, int);
extern FILE *cht_sfreopen(char *, char *, FILE *, int);
extern int cht_fnew(char *);
extern char *cht_mktemp(char *, const int);
extern void cht_crepeat(char, FILE *, int);
extern void cht_curtime(int []);
extern void cht_filter(char *, int);
extern char *cht_strsave(char *, int);
extern char *cht_strnsave(char *, int, int);
extern char *cht_splitf(char *, char);
extern char *cht_strunc(char *);
extern char *cht_strcpy(char *, char *);
extern void cht_tpm_data(int);
extern int cht_putenv(char *, char *);
extern long cht_dtol(double);
extern int cht_range(double, double, double);
extern long cht_combs(int, int);
extern int cht_gauss(double *, int, double *);
extern long cht_lrand(long, long);
extern double cht_drand(double, double);
extern void cht_trns(int, int);
extern int cht_numck(char *, long, long);
extern int cht_ulnumck(char *, unsigned long, unsigned long);
#define CHT_NUMCK_DIGIT 1
#define CHT_NUMCK_RANGE 2
extern int cht_safmck(char *);
extern int cht_nafmck(long);
extern long cht_fixafm(char *);
extern int cht_prterm(int);
extern char *cht_diekat(char *, char *, char *, char *, char *, char *);
extern char *cht_arenthili(char *, char *);
extern char *cht_thiliaren(char *, char *);

#define CHT_PTOSI_ONOMASTIKI 1
#define CHT_PTOSI_GENIKI 2
#define CHT_PTOSI_ETIATIKI 3
extern char *cht_onoma(char *, char *, const char, const int);
extern char *cht_lexposo(char *, long);
extern char *cht_lexevro(char *, long);
extern char *cht_spellevro(char *, double);
extern char *cht_triad(char *, double, int, int);
extern char *cht_fltprint(char *, double);
extern long cht_round(double);
extern char *cht_cfill(char *, char, int);

#define CHT_EXPIRE_DATE 10, 28, 2020
#define cht_zero(x) (cht_range((double)(x), -0.5, 0.5))

#endif
