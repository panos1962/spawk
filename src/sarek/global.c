#include "sarek.h"

char *sarek_progname = "sarek";
MYSQL *sarek_connection = NULL;
MYSQL_RES *sarek_result = NULL;
int sarek_ncol = 0;
char sarek_message[MAX_MESSAGE_LEN + 2];

char *sarek_user = NULL;
char *sarek_password = NULL;
char *sarek_database = NULL;
char *sarek_host = NULL;
char *sarek_charset = NULL;
char *sarek_null = "";
int sarek_debug = 0;

char sarek_eom = '\004';		// message delimiter (Control-D)
char sarek_sep = '\011';		// field separator (tab character)
char sarek_eod = '\004';		// end of data (Control-D)
