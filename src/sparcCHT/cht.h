extern char *cht_progname;

extern char *cht_set_progname(char *);
extern void cht_error(char *);
extern void cht_fatal(char *, int);
extern int cht_erropt(void);
extern void cht_nomem(char *, int);
extern char *cht_strsave(char *, int);
extern char *cht_strnsave(char *, int, int);
extern char *cht_strcpy(char *, char *);
extern FILE *cht_sfreopen(char *, char *, FILE *, int);
extern void cht_filter(char *, int);
