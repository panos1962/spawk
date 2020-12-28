#include "sarek.h"
#include <unistd.h>
#include <string.h>

static void database_connect(void);
static void debug_connection(void);
static void failed_query(void);
static void dmlddl_query(void);
static void result_query(const int);
static void process_message(char *);
static void password(char *);

void sarek(void) {
	int fis;
	char *query;

	if (sarek_debug)
	fprintf(stderr, "MESSAGE: >>%s<<\n", sarek_message);

	// Empty messages are allowed but do nothing.

	if (!(*sarek_message))
	return;

	/////////////////////////////////////////////////////////////////
	//                         DIRECTIVES                          //
	/////////////////////////////////////////////////////////////////

	// Directives begin with a "@" character.

	if (*sarek_message == '@')
	return process_message(sarek_message + 1);

	/////////////////////////////////////////////////////////////////
	//                          QUERIES                            //
	/////////////////////////////////////////////////////////////////

	// SQL query message detected. There must be no results remained
	// from previous queries, in order to process the incoming query.

	if (sarek_result)
	sarek_fatal("data surplus", EXIT_DATA_SURPLUS);

	// Field indexing scheme (fis) code has been inserted before the
	// submitted query as 0, 1, 2 or 3, so it has to be the first
	// character of the "sarek_message". Now it's time to extract
	// this character from the "sarek_message".

	switch (fis = (*sarek_message - '0')) {
	case 0:		// row[0] = ROW
	case 1:		// row[0] = ROW, row[1] = F1, row[2] = F2,...
	case 2:		// row[""] = ROW, row[C1] = F1, row[2] = F2,...
	case 3:		// both of 1 and 2
		break;
	default:
		sarek_fatal("invalid field indexing scheme", EXIT_FIS);
	}

	// Actual SQL query starts after the fis character, that is one
	// character after the first character of the message.
	// Empty queries are allowed but do nothing.

	if (!*(query = sarek_message + 1))
	return;

	if (!sarek_connection)
	database_connect();

	// It's time to pass the query to the SQL server for execution.
	// On error, code 1 will be returned to the caller.

	if (mysql_query(sarek_connection, query))
	return failed_query();	// 1: error

	// Query succeeded.
	// No errors allowed!

	if (mysql_errno(sarek_connection))
	sarek_sqlfatal("MySQL: mysql_query failed", EXIT_QUERY);

	// Check for query results. If there is no result set available,
	// (even an empty one), then the submitted query is DML/DDL and
	// code 2 will be returned to the caller.

	if (!(sarek_result = mysql_use_result(sarek_connection)))
	return dmlddl_query();	// 2: DML/DQL, no result set

	// Result set has been produced. The result set may be empty but
	// that makes no difference.

	// Number of columns of the result rows will be needed when fetching
	// the rows and return them to the caller.

	if ((sarek_ncol = mysql_num_fields(sarek_result)) <= 0)
	sarek_fatal("zero number of selected fields", EXIT_ZERO_NF);

	// Result set has been prepared and the rows must be fetched from the
	// caller, or  the set must be freed. Code 3 must be returned.

	result_query(fis);	// 3: DQL, result set ready (even empty)
}

static void database_connect(void) {
	if (sarek_connection)
	sarek_fatal("database connection already open", EXIT_CONNECT);

	if (!(sarek_connection = mysql_init(NULL)))
	sarek_fatal("MySQL: mysql_init failed", EXIT_INIT);

	if (mysql_options(sarek_connection, MYSQL_READ_DEFAULT_GROUP, "spawk"))
	sarek_sqlfatal("MySQL: mysql_options failed", EXIT_CONNECT);

	if (*sarek_user == '\0')
	sarek_user = NULL;

	if (*sarek_password == '\0')
	sarek_password = NULL;

	else if (!strcmp(sarek_password, EMPTY_PASSWORD))
	sarek_password = "";

	if (!sarek_charset)
	sarek_charset = "utf8";

	if (sarek_debug)
	debug_connection();

	if (!mysql_real_connect(sarek_connection, sarek_host, sarek_user,
		sarek_password, sarek_database, 0, NULL, 0))
	sarek_sqlfatal("MySQL: mysql_real_connect failed", EXIT_CONNECT);

	if (mysql_set_character_set(sarek_connection, sarek_charset))
	sarek_sqlfatal("MySQL: mysql_set_character_set failed", EXIT_CHARSET);
}

static void debug_connection(void) {
	fprintf(stderr, "HOST: >>%s<<\n", sarek_host ? sarek_host : "NULL");
	fprintf(stderr, "USER: >>%s<<\n", sarek_user ? sarek_user : "NULL");
	fprintf(stderr, "PASSWORD: >>%s<<\n", sarek_password ? sarek_password : "NULL");
	fprintf(stderr, "DATABASE: >>%s<<\n", sarek_database ? sarek_database : "NULL");
	fprintf(stderr, "CHARSET: >>%s<<\n", sarek_charset ? sarek_charset : "NULL");
	fprintf(stderr, "NULLSTR: >>%s<<\n", sarek_null ? sarek_null : "NULL");
}

// Function "failed_query" is called on error while processing an SQL query.
// A code of 1 is returned to the caller, along with the MySQL errno/error.

static void failed_query(void) {
	response_begin(1);
	printf("%u%c%s", mysql_errno(sarek_connection),
		sarek_sep, mysql_error(sarek_connection));
	response_end();
}

// Function "dmlddl_query" is called on receiving DML/DDL queries where no
// result set will be created. A code of 2 is returned to the caller, along
// with the MySQL errno/error, affected rows, last inserted ID and MySQL
// info message.

static void dmlddl_query(void) {
	my_ulonglong affected;
	char *info;

	if (mysql_errno(sarek_connection))
	sarek_sqlfatal("MySQL: mysql_use_result failed", EXIT_USE_RESULT);

	affected = mysql_affected_rows(sarek_connection);

	if (affected == (my_ulonglong)(-1))
	affected = 0L;

	info = (char *)mysql_info(sarek_connection);

	if (info == NULL)
	info = "";

	response_begin(2);
	printf("%llu%c%llu%c%s", affected, sarek_sep,
		mysql_insert_id(sarek_connection), sarek_sep, info);
	response_end();
}

// Function "result_query" is called on receiving DQL queries where result
// set will be created (even empty). A code of 1 is returned to the caller,
// along with the MySQL errno/error.

static void result_query(const int fis) {
	MYSQL_FIELD *f;

	response_begin(3);
	printf("%d", fis);

	if (fis & 2) {
		while (f = mysql_fetch_field(sarek_result))
		printf("%c%s", sarek_sep, f->name);
	}

	response_end();
}

static void process_message(char *message) {
	switch (*message) {

	// In order to fetch next row from the result set a "@F" message
	// must be received from the caller.

	case 'F':
	return print_row();

	// In order to release the result set and get ready to receive a
	// new query, a "@R" message must be received from the caller.

	case 'R':
	return free_result();

	case 'S':
	return setup(message + 1);

	case 'P':
	return password(message + 1);

	}

	sarek_error(NULL);
	fprintf(stderr, "%s: unknown message\n", message);
	exit(EXIT_UNKNOWN_MESSAGE);
}

static void password(char *message) {
	char *p;

	if (*message == '\0')
	message = "password: ";

	while ((p = getpass(message)) == NULL)
	;

	puts(p);
	fflush(stdout);
}
