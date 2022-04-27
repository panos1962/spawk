#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>

#include <mysql.h>

#include <gawkapi.h>

int plugin_is_GPL_compatible;
static const gawk_api_t *api;
static awk_ext_id_t ext_id;
static const char *ext_version = "spawk gawk extension: version 1.9";

static awk_bool_t init_spawk(void);
static awk_bool_t (*init_func)(void) = init_spawk;

//////////////////////////////////////////////////////////////////////////////@

// Objects of type COLNAME represent columns' names. A string seems to be
// suffice, but we'll need the length of the name repeatedly as well, so we
// define this type for keeping the length along with the name.

typedef struct {
	char *col;
	size_t len;
} COLNAME;

// Objects of type SERVER act as database servers, as each SERVER object
// accepts SQL queries and try to execute these queries. Actually, each SERVER
// object, contains a database connection and passes submitted queries to the
// main database server, but from gawk's point of view, SERVER objects act as
// database servers themselves.

typedef struct {
	MYSQL *connection;	// database server connection
	unsigned short fis;	// fields indexing scheme
	MYSQL_RES *result;	// result rows provider
	unsigned int ncol;	// number of selected columns
	COLNAME *cols;		// list of selected columns names
} SERVER;

// There exists a server stack. For each submitted query a new server is pushed
// in the stack and executes the given query. While the query is active, the
// server remains in the stack, meaning that if another query is given while
// the first query is still active, a new server will be pushed in the stack,
// giving an illusion of running nested queries, though there is no way to run
// nested queries using a single database connection. Recall that by the word
// "server" we actually mean a database connection.
//
// In other words there may exist many active queries, but as new queries are
// becoming active (top server), all other active queries are being suspended.
// After finishing with an active query, the top server is popped off the stack,
// but the corresponding database connection remains open for future queries.
//
// For DDL and DML queries a server is pushed in the stack, run the query and
// then popped off the stack. For DQL queries, aka queries that produce result
// rows, a server is pushed and then the result rows must be retrieved, or
// freed for the corresponding server to be released and popped off the stack.

static SERVER *slist;

// Variable nconn is the count of active servers at any time. When a server is
// pushed in the stack, nconn increases, while when a server is popped off the
// stack, nconn decreases. At any given time, the current active server is the
// server[nconn - 1], except when nconn is zero where no active servers exist.

static int nconn;

// Variable maxconn is the maximum number of database servers (connections)
// allowed for spawk. This value defaults to 10, but it can be set from the
// command line using the "spawk_maxconn" variable.

static int maxconn;

static MYSQL *dbconnect(void);
static int fetchrow(const char *, int);
static short set_fis(SERVER *, const char *fis);
static void set_cols(SERVER *);
static void ddldml_data(SERVER *);
static void reset_sqlerror(void);
static void print_sqlerror(SERVER *);
static void cleanup(void *, int);
static int reset_servers(void);
static void server_init(SERVER *);
static int server_close(SERVER *);
static void server_pop(void);
static void free_result(SERVER *);
static char *strnsave(char *, size_t);
static char *stradd(char *, char *);
static void varset(char *, char *, awk_value_t *);

//////////////////////////////////////////////////////////////////////////////@

static awk_bool_t init_spawk(void) {
	awk_value_t val;

	// Variable "spawk_null" is the default NULL column value
	// representaion string (Control-N).

	if (!sym_lookup("spawk_null", AWK_STRING, &val)) {
		make_const_string("\016", 1, &val);
		varset("spawk_init", "spawk_null", &val);
	}

	// Variable "spawk_verbose" acts as a numeric flag. Non-zero
	// values cause error error messages to be printed to the
	// standard error for failed queries (default).

	if (!sym_lookup("spawk_verbose", AWK_STRING, &val))
	varset("spawk_init", "spawk_verbose", make_number(1.0, &val));

	// Variable "spawk_maxconn" contains the maximum number of
	// database servers (connections). Can be set only from the
	// command line. If not set then defaults to 10.

	if (!sym_lookup("spawk_maxconn", AWK_NUMBER, &val))
	varset("spawk_init", "spawk_maxconn", make_number(10.0, &val));

	maxconn = val.num_value;
	if ((maxconn < 1) || (maxconn > 100))
	fatal(ext_id, "spawk_init: invalid maxconn value");

	if (!(slist = gawk_calloc(maxconn, sizeof(SERVER))))
	fatal(ext_id, "spawk_init: out of memory");

	// Reset server attributes for all servers.

	for (nconn = 0; nconn < maxconn; nconn++)
	server_init(slist + nconn);

	// Reset server counter.

	nconn = 0;

	// At exit it's good to do some cleanup jobs like close
	// any open database connections etc.

	awk_atexit(cleanup, NULL);

	return 1;
}

// Function spawk_submit submits queries to the database server. A new
// database server is pushed in the server stack and remains there until
// the query is finished. For DDL/DML queries there is no need for the
// server to remain in the stack, but for DQL queries the server remains
// in the stack until all of the result rows are retrieved, or the result
// is freed.
//
//	spawk_submit(query [, fis])
//
// Parameters
// ----------
//
// query	The SQL query to be sumbitted (as a string).
//
// fis		The "field indexing scheme" (fis). Field indexing scheme
//		controls the result row array indexing. Default fis is 1,
//		while valid fis values are 0, 1, 2 and 3. There are also
//		string equivalents, namely NONE, NUM, ASSOC and BOTH.
//
// Return value
// ------------
//
// 1	An error occured. No results, server popped off the stack. The
//	spawk_sqlerrno and spawk_sqlerror variables are set.
//
// 2	DDL/DML query detected. No results, server popped off the stack.
//
// 3	DQL query detected. Rseult rows produced, server remains in the stack
//	until all of the rows are retrieved, or the result is freed.
//
// Description
// -----------
// Function spawk_submit is used to submit SQL queries to the database server.
// Whenever spawk_submit is called, a new server is pushed in the server stack.
// Speaking of servers we actually mean database connections. Whenever a new
// connection is created, it remains in the stack until the program exits, or
// stack reset using spawk_reset.
//
// There exist three options:
//
// Query failed to execute. In this case the spawk_sqlerrno and spawk_sqlerror
// variables are set and the server is popped off the stack.
//
// Query is a DDL/DML query. DDL and DML queries do not produce result rows.
// In case of DML queries, the spawk_affected and spawk_insertid variables are
// set; spawk_info variable is also set for some massive inserts or upates.
//
// Query is of DQL type. DQL queries produce result rows (even empty). These
// rows must be retrieved, or the result must be freed, for the server to be
// pushed off the stack. If a new query is submitted while the server is still
// in the stack, a new server will be pushed in the stack to execute the new
// query.
//
// Filed Indexing Scheme (fis)
// ---------------------------
//
// In case of DQL queries, the result rows will be retrieved later using the
// so called "return array". Every row will be returned as an array using an
// indexing scheme, according with the fis given. Default fis is 1.
//
//	0	No column values are returned. Given an index in fetchrow or
//		fetchone later calls, the whole row will be returned. In this
//		case, columns are separated by spawk_OFS strings.
//
//	1	Numeric indexing, no whole row is returned, e.g.
//		SELECT `login`, `name`, `email` FROM `user` will return
//		"login" value indexed as 1, "name" value indexed as 2 and
//		"email" value indexed as 3.
//
//	2	Associative indexing, no whole row is returned, e.g. in
//		SELECT `login`, `name`, `email` FROM `user` will return
//		"login" value indexed as "login", "name" value indexed as
//		"name" and "email" value indexed as "email".
//
//	3	Both of numeric and associative indices are used. No whole
//		row is returned.
//
//	NONE	Same as 0
//
//	NUM	Same as 1
//
//	ASSOC	Same as 2
//
//	BOTH	Same as 3

static awk_value_t *
do_submit(int nargs, awk_value_t *result, struct awk_ext_func *unused) {
	SERVER *server;
	awk_value_t queryarg;
	awk_value_t fisarg;
	char *fis;

	assert(result != NULL);

	if (nargs < 1)
	fatal(ext_id, "spawk_submit: missing query");

	if (nargs > 2)
	fatal(ext_id, "spawk_submit: too many arguments");

	// First argument is the SQL query.

	if (!get_argument(0, AWK_STRING, &queryarg))
	fatal(ext_id, "spawk_submit: invalid query");

	// Second argument is the field indexing scheme (fis).

	if (nargs > 1) {
		if (!get_argument(1, AWK_STRING, &fisarg))
		fatal(ext_id, "spawk_submit: failed to get fis");

		else
		fis = fisarg.str_value.str;
	}

	else
	fis = NULL;

	if (nconn >= maxconn)
	fatal(ext_id, "spawk_submit: out of servers");

	server = slist + nconn;

	// If this is the first time pushed in the stack, open a database
	// connection.

	if (!server->connection)
	server->connection = dbconnect();

	set_fis(server, fis);

	// It's time to submit the given query. First of all, reset SQL
	// error flags.

	reset_sqlerror();

	// On error return 1

	if (mysql_query(server->connection, queryarg.str_value.str)) {
		print_sqlerror(server);
		return make_number(1.0, result);
	}

	// Ensure no error have been set!

	if (mysql_errno(server->connection))
	fatal(ext_id, "spawk_submit: unexpected error");

	// Return 2 for DDL/DML queries. No result rows have been produced.

	if (!(server->result = mysql_use_result(server->connection))) {
		ddldml_data(server);
		return make_number(2.0, result);
	}

	// Return 3 for DQL queries. Result rows have been produced and must
	// be fetched or freed.

	if ((server->ncol = mysql_num_fields(server->result)) <= 0)
	fatal(ext_id, "spawk_submit: invalid number of selected fields");

	// If the field indexing scheme involves columns' names, then get and
	// store these names.

	if (server->fis & 2)
	set_cols(server);

	// Increase stack level, until all of the result rows are fetched or
	// freed.

	nconn++;
	return make_number(3.0, result);
}

// Function spawk_fetchrow returns the next row from the result rows produced.
//
//	spawk_fetchrow(array [, idx])
//
// Parameters
// ----------
// Function spawk_fetchrow accepts an array argument which will be returned
// filled with the selected column values, according with the specified field
// indexing scheme (fis).
//
// If given a second parameter, then this parameter is used as index for the
// whole row.
//
// Return value
// ------------
// The number of selected columns is returned. If there are no result rows
// remaining, then the result is freed, the servre is popped off the stack
// and 0 is returned.

static awk_value_t *
do_fetchrow(int nargs, awk_value_t *result, struct awk_ext_func *unused) {
	assert(result != NULL);
	return make_number((double)fetchrow("spawk_fetchrow", nargs), result);
}

// Function spawk_fetchone is just the same with spawk_fetchrow, but the
// result is freed and the server is popped off the stack, even if there
// exist more rows remaining in the result set.

static awk_value_t *
do_fetchone(int nargs, awk_value_t *result, struct awk_ext_func *unused) {
	int n;

	assert(result != NULL);

	if (n = fetchrow("spawk_fetchone", nargs))
	server_pop();

	return make_number((double)n, result);
}

static awk_value_t *
do_freerest(int nargs, awk_value_t *result, struct awk_ext_func *unused) {
	assert(result != NULL);

	if (nconn <= 0)
	fatal(ext_id, "spawk_freerest: data shortage");

	server_pop();

	return make_number((double)0, result);
}

static awk_value_t *
do_reset(int nargs, awk_value_t *result, struct awk_ext_func *unused) {
	assert(result != NULL);
	return make_number((double)reset_servers(), result);
}

// Function spawk_escape scans passed string and escapes single quotes
// and backslashes. Other special characters are also escaped (newlines,
// tabs formfeeds etc).
// The escaped string is returned inside single quotes, except if there
// exists a second parameter of zero value.

static awk_value_t *
do_escape(int nargs, awk_value_t *result, struct awk_ext_func *unused) {
	awk_value_t sval;
	awk_value_t qval;
	int quote;
	SERVER *server;
	unsigned long l;
	char *t1;
	char *t2;
	char *p;
	char *q;

	assert(result != NULL);

	if (nargs < 1)
	fatal(ext_id, "spawk_escape: missing argument");

	get_argument(0, AWK_STRING, &sval);

	quote = ((nargs > 1) && get_argument(1, AWK_STRING, &qval)) ?
		strcmp(qval.str_value.str, "0") : 1;

	if (nconn >= maxconn)
	fatal(ext_id, "spawk_escape: out of servers");

	server = slist + nconn;

	if (!server->connection)
	server->connection = dbconnect();

	// First pass (escape special characters via MySQL function).

	emalloc(t1, char *, ((l = strlen(sval.str_value.str)) * 2) + 3, "spawk_escape");
	p = t1;

	if (quote)
	*p++ = '\'';

	p += mysql_real_escape_string(server->connection, p, sval.str_value.str, l);

	if (quote)
	*p++ = '\'';

	*p = '\0';

	for (q = t1; *q; q++) {
		if (*q == '\t')
		break;
	}

	// End of string has been reached; no tabs detected.

	if (*q != '\t')
	return make_malloced_string(t1, p - t1, result);

	// Tabs detected; need for second pass.

	// Second pass (escape tab characters).
	// Extra cost about 20% slower.
	// Reason: mysql_real_escape_string does not escape tab characters :-(

	emalloc(t2, char *, l * 2, "spawk_escape");
	for (p = t1, q = t2; *p; p++, q++) {
		if (*p != '\t') {
			*q = *p;
			continue;
		}

		*q++ = '\\';
		*q = 't';
	}

	gawk_free(t1);
	*q = '\0';
	return make_malloced_string(t2, q - t2, result);
}

static awk_value_t *
do_getpass(int nargs, awk_value_t *result, struct awk_ext_func *unused) {
	char *prompt = "password: ";
	awk_value_t val;
	char *password;

	assert(result != NULL);

	if ((nargs > 0) && get_argument(0, AWK_STRING, &val))
	prompt = val.str_value.str;

	while ((password = getpass(prompt)) == NULL)
	;

	return make_const_string(password, strlen(password), result);
}

static awk_ext_func_t func_table[] = {
	{ "spawk_submit", do_submit, 2, 1, awk_false, NULL },
	{ "spawk_fetchrow", do_fetchrow, 2, 1, awk_false, NULL },
	{ "spawk_fetchone", do_fetchone, 2, 1, awk_false, NULL },
	{ "spawk_freerest", do_freerest, 0, 0, awk_false, NULL },
	{ "spawk_reset", do_reset, 0, 0, awk_false, NULL },
	{ "spawk_escape", do_escape, 2, 1, awk_false, NULL },
	{ "spawk_getpass", do_getpass, 1, 0, awk_false, NULL },
};

//////////////////////////////////////////////////////////////////////////////@

static MYSQL *dbconnect(void) {
	MYSQL *conn;
	awk_value_t val;
	awk_value_t sesami;
	awk_value_t idx;
	char *dbhost;
	char *dbuser;
	char *dbpassword;
	char *dbname;
	char *dbcharset;

	if (!(conn = mysql_init(NULL)))
	fatal(ext_id, "spawk: MySQL: mysql_init failed");

	if (mysql_options(conn, MYSQL_READ_DEFAULT_GROUP, "spawk"))
	fatal(ext_id, "spawk: MySQL: mysql_options failed");

	dbhost = NULL;
	dbuser = NULL;
	dbpassword = NULL;
	dbname = NULL;
	dbcharset = NULL;

	if (sym_lookup("spawk_sesami", AWK_ARRAY, &sesami)) {
		make_const_string("dbhost", 6, &idx);
		if (get_array_element(sesami.array_cookie, &idx, AWK_STRING, &val))
		dbhost = val.str_value.str;

		make_const_string("dbuser", 6, &idx);
		if (get_array_element(sesami.array_cookie, &idx, AWK_STRING, &val))
		dbuser = val.str_value.str;

		make_const_string("dbpassword", 10, &idx);
		if (get_array_element(sesami.array_cookie, &idx, AWK_STRING, &val))
		dbpassword = val.str_value.str;

		make_const_string("dbname", 6, &idx);
		if (get_array_element(sesami.array_cookie, &idx, AWK_STRING, &val))
		dbname = val.str_value.str;

		make_const_string("dbcharset", 9, &idx);
		if (get_array_element(sesami.array_cookie, &idx, AWK_STRING, &val))
		dbcharset = val.str_value.str;
	}

	if (!mysql_real_connect(conn, dbhost, dbuser, dbpassword, dbname, 0, NULL, 0))
	fatal(ext_id, "spawk: MySQL: mysql_real_connect failed");

	if (dbcharset && mysql_set_character_set(conn, dbcharset))
	fatal(ext_id, "spawk: MySQL: mysql_set_character_set failed");

	return conn;
}

static void ddldml_data(SERVER *server) {
	my_ulonglong affected;
	my_ulonglong insertid;
	char *info;
	awk_value_t val;

	affected = mysql_affected_rows(server->connection);

	if (affected == (my_ulonglong)(-1))
	affected = 0L;

	varset("spawk_submit", "spawk_affected", make_number((double)affected, &val));

	insertid = mysql_insert_id(server->connection);
	varset("spawk_submit", "spawk_insertid", make_number((double)insertid, &val));

	info = (char *)mysql_info(server->connection);

	if (info == NULL)
	info = "";

	make_const_string(info, strlen(info), &val);
	varset("spawk_submit", "spawk_info", &val);
}

static int fetchrow(const char *func, int nargs) {
	awk_value_t array_param;
	awk_value_t rowidx_param;
	awk_array_t array;
	awk_value_t key;
	awk_value_t val;
	SERVER *server;
	MYSQL_ROW row;
	size_t rowlen;
	size_t collen;
	char *rowidx;
	char *prow;
	char *null;
	char *ofs;
	char *p;
	int i;

	if (nargs > 2)
	fatal(ext_id, "%s: too many arguments passed", func);

	if (nargs < 1)
	fatal(ext_id, "%s: missing argument", func);

	if (nconn <= 0)
	fatal(ext_id, "%s: data shortage", func);

	if (!sym_lookup("spawk_null", AWK_STRING, &val))
	fatal(ext_id, "spawk_null: undefined");

	null = val.str_value.str;

	if (sym_lookup("spawk_OFS", AWK_STRING, &val))
	ofs = val.str_value.str;

	else if (sym_lookup("OFS", AWK_STRING, &val))
	ofs = val.str_value.str;

	else
	ofs = "\t";		// tab character

	if (nargs > 0) {
		if (!get_argument(0, AWK_ARRAY, &array_param))
		fatal(ext_id, "%s: bad array parameter", func);

		array = array_param.array_cookie;
		clear_array(array);
	}

	server = slist + (nconn - 1);

	if (!(row = mysql_fetch_row(server->result))) {
		server_pop();
		return 0;
	}

	if (nargs > 1) {
		if (!get_argument(1, AWK_STRING, &rowidx_param))
		fatal(ext_id, "%s: bad row index parameter", func);

		rowidx = rowidx_param.str_value.str;
	}

	else
	rowidx = NULL;

	if ((!server->fis) && (!rowidx))
	return server->ncol;

	// If the row as a whole is returned, then we must allocate
	// space for the whole row.

	if (rowidx)
	rowlen = (server->ncol - 1) * strlen(ofs);

	for(i = 0; i < server->ncol; i++) {
		if (!row[i])
		row[i] = null;

		collen = strlen(row[i]);

		if (rowidx)
		rowlen += collen;

		// Number indexing scheme

		if (server->fis & 1) {
			char si[64];

			sprintf(si, "%d", i + 1);
			set_array_element(array, make_const_string(si, strlen(si), &key),
				make_const_string(row[i], collen, &val));
		}

		// Associative indexing scheme

		if (server->fis & 2)
		set_array_element(array,
			make_const_string(server->cols[i].col, server->cols[i].len, &key),
			make_const_string(row[i], collen, &val));
	}

	if (!rowidx)
	return server->ncol;

	if (!(prow = gawk_malloc(rowlen + 1)))
	fatal(ext_id, "%s: out of memory", func);

	for (p = stradd(prow, row[0]), i = 1; i < server->ncol; i++) {
		p = stradd(p, ofs);
		p = stradd(p, row[i]);
	}

	*p = '\0';
	set_array_element(array, make_const_string(rowidx, strlen(rowidx), &key),
		make_const_string(prow, p - prow, &val));

	gawk_free(prow);
	return server->ncol;
}

static short set_fis(SERVER *server, const char *fis) {
	if (!fis)
	return (server->fis = 1);

	if (!strcmp(fis, "0"))
	return (server->fis = 0);

	if (!strcmp(fis, "1"))
	return (server->fis = 1);

	if (!strcmp(fis, "2"))
	return (server->fis = 2);

	if (!strcmp(fis, "3"))
	return (server->fis = 3);

	if (!strcmp(fis, "NONE"))
	return (server->fis = 0);

	if (!strcmp(fis, "NUM"))
	return (server->fis = 1);

	if (!strcmp(fis, "ASSOC"))
	return (server->fis = 2);

	if (!strcmp(fis, "BOTH"))
	return (server->fis = 3);

	fatal(ext_id, "spawk: set_fis: invalid fis");
}

static void set_cols(SERVER *server) {
	int i;
	MYSQL_FIELD *col;

	if (!(server->cols = gawk_calloc(server->ncol, sizeof(COLNAME))))
	fatal(ext_id, "spawk: set_cols: out of memory");

	for (i = 0; col = mysql_fetch_field(server->result); i++) {
		server->cols[i].len = strlen(col->name);
		server->cols[i].col = strnsave(col->name, server->cols[i].len);
	}
}

static void reset_sqlerror(void) {
	awk_value_t val;

	varset("spawk: reset_sqlerror", "spawk_sqlerrno", make_number(0.0, &val));
	varset("spawk: reset_sqlerror", "spawk_sqlerror", make_const_string("", 0, &val));
}

static void print_sqlerror(SERVER *server) {
	awk_value_t verbose;
	awk_value_t val_errno;
	awk_value_t val_error;
	unsigned int sql_errno;
	char *sql_error;

	sql_errno = mysql_errno(server->connection);
	make_number((double)sql_errno, &val_errno);
	varset("spawk: print_sqlerror", "spawk_sqlerrno", &val_errno);

	sql_error = (char *)mysql_error(server->connection);
	make_const_string(sql_error, strlen(sql_error), &val_error);
	varset("spawk: print_sqlerror", "spawk_sqlerror", &val_error);

	if (!sym_lookup("spawk_verbose", AWK_STRING, &verbose))
	return;

	if (!strcmp(verbose.str_value.str, "0"))
	return;

	nonfatal(ext_id, "%u: %s", sql_errno, sql_error);
}

static void cleanup(void *data, int exit_status) {
	reset_servers();
}

static int reset_servers(void) {
	int n;

	for (n = 0, nconn = 0; nconn < maxconn; nconn++)
	n += server_close(slist + nconn);

	nconn = 0;
	return n;
}

static void server_init(SERVER *server) {
	server->connection = NULL;
	server->fis = 1;
	server->result = NULL;
	server->ncol = 0;
	server->cols = NULL;
}

static int server_close(SERVER *server) {
	free_result(server);

	if (!server->connection)
	return 0;
	mysql_close(server->connection);
	server->connection = NULL;

	return 1;
}

void server_pop(void) {
	nconn--;
	free_result(slist + nconn);
}

static void free_result(SERVER *server) {
	if (server->result) {
		mysql_free_result(server->result);
		server->result = NULL;
	}

	if (server->cols) {
		int i;

		for (i = 0; i < server->ncol; i++)
		gawk_free(server->cols[i].col);

		gawk_free(server->cols);
		server->cols = NULL;
		server->ncol = 0;
	}
}

static char *strnsave(char *s, size_t l) {
	char *p;

	if (!s)
	return NULL;

	if (!(p = gawk_malloc(l + 1)))
	fatal(ext_id, "spawk: strnsave: out of memory");

	memcpy(p, s, l);
	*(p + l) = '\0';

	return p;
}

static char *stradd(char *t, char *s) {
	while (*s)
	*t++ = *s++;

	return t;
}

static void varset(char *func, char *var, awk_value_t *val) {
	if (!sym_update(var, val))
	fatal(ext_id, "%s: %s: cannot set", func, var);
}

dl_load_func(func_table, spawk, "")
