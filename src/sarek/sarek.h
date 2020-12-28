#include <stdlib.h>
#include <stdio.h>
#include <my_global.h>
#include <mysql.h>

#define EXIT_USAGE 1
#define EXIT_INIT 2
#define EXIT_CONNECT 3
#define EXIT_HUGE_MESSAGE 4
#define EXIT_USE_RESULT 5
#define EXIT_DATA_SURPLUS 6
#define EXIT_DATA_SHORTAGE 7
#define EXIT_ZERO_NF 8
#define EXIT_FREE_RESULT 9
#define EXIT_CHARSET 10
#define EXIT_QUERY 11
#define EXIT_FIS 12
#define EXIT_UNKNOWN_MESSAGE 13
#define EXIT_SETUP 14
#define EXIT_MEMORY 15

#define MAX_MESSAGE_LEN 100000

// In order to use database privileges with empty passwords, we
// accept the following convention between "sarek" and "spawk.awk":
// They both use an arbitrary string for denoting empty password.
// If you wonder about the bizarre string, it's the SHA1 of "SPAWK".

#define EMPTY_PASSWORD "d0d148e9af51b22184758e53fe4d3e33a6e44fa9"

extern char *sarek_progname;
extern MYSQL *sarek_connection;
extern MYSQL_RES *sarek_result;
extern int sarek_ncol;
extern char sarek_message[];

extern char *sarek_user;
extern char *sarek_password;
extern char *sarek_database;
extern char *sarek_host;
extern char *sarek_charset;
extern char *sarek_null;
extern int sarek_debug;

extern char sarek_eom;
extern char sarek_sep;
extern char sarek_eod;

extern int main(int, char *[]);
extern void sarek(void);
extern char *get_message(void);
extern void response_begin(const int);
extern void response_end(void);
extern void response_eod(void);
extern void print_row(void);
extern void free_result(void);
extern void setup(char *);
extern void debug_mode(void);
extern void sarek_error(const char *);
extern void sarek_fatal(const char *, int);
extern void sarek_sqlerror(char *);
extern void sarek_sqlfatal(char *, int);
