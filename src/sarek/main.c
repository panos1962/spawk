//////////////////////////////////////////////////////////////////////////////
//
// sarek - SPAWK dedicated database server
//
// Copyright (C) 2004-2017, by Panos Papadopoulos
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Name: Panos Papadopoulos
// Email: panos1962_AT_gmail_DOT_com
//
//////////////////////////////////////////////////////////////////////////////
// 
// While Sarek and Amanda were Spock's parents, "sarek" executable is
// SPAWK's dedicated database server, and in this sense "sarek" is like
// a father for SPAWK. In fact, "sarek" is a custom MySQL database client
// similar to the standard "mysql" command-line tool.
//
// "sarek" accepts messages from the standard input and prints results to
// the standard output, so "sarek" acts as a server for AWK. On the other
// hand, "sarek" passes queries to the dabase server and gets results from
// the server, so "sarek" acts as a client for the database server. More
// than that, "sarek" can be seen as a database driver for communicating
// with the database server.
//
// As have been said earlier, "sarek" accepts messages from the standard input.
// Messages are directives or SQL queries, while results are items returned
// from DQL queries (SELECT, SHOW, DESCRIBE etc). Besides result rows (data),
// "sarek" prints a line of useful information for every given submitted query
// (metadata).
//
// After printing (or rejecting) all of the result rows, "sarek" signals
// end of data by printing an EOD character (defaults to [Control-D]).
// The core loop of "sarek" executable is as follows:
//
//	while there is a new message
//	message --> sarek engine --> metadata/data/EOD
//
// Messages may be of type "query" or "directive". Messages are read from
// the standard input. End of message is signaled by the EOM character which
// defaults to [Control-D]; whenever an EOM character is detected in the
// standard input, the text already read until EOM is checked and if it is
// a directive, then it is executed by "sarek" itself, or else it is considered
// to be an SQL query and it is submitted to the database server for execution.
//
// As said, non-directive type messages are considered to be of type query.
// Query-type messages follow a very strict syntax: the first character of the
// query-type message msut be either 0, 1, 2 or 3 denoting the field indexing
// scheme (fis) for the resulting rows; for non-DQL queries the fis will be
// ignored, but it still must be the very first character of the message.
// The rest of the message is the actual SQL query which will be passed to
// the database server for execution. After processing a new query, the
// database server returns useful information to "sarek" and there exist the
// following possibilities:
//
// 1. Query failed to execute
// --------------------------
// In that case, "sarek" prints 1, MySQL errno and MySQL error to the standard
// output. The three items are being separated by OFS characters (defaults to
// tab). MySQL errno is a number denoting the type of the error, while MySQL
// error is the corresponding error message, e.g.
//
//	1<tab>1064<tab>You have an error in your SQL syntax; check...
//
// 2. DML/DDL query submitted
// --------------------------
// Queries are of two types: either DQL, that is queries beginning with the
// SELECT, SHOW, DESCRIBE etc keywords, or non DQL, that is DML or DDL queries.
// Non DQL queries are queries that produce no result set; result set is a set
// of rows produced by DQL queries, even if this is an empty set. In contrary,
// non DQL queries produce no result set at all. These queries are even DML
// queries (INSERT, UPDATE, DELETE etc), or DDL queries (CREATE, DROP etc).
// In case of DML/DDL queries, "sarek" prints 2, MySQL errno, MySQL error,
// affected rows, last inserted row ID and, finally, MYSQL info e.g.
//
//	2<tab>0<tab><tab>1<tab>1<tab><tab>12788122
//
// 3. DQL query submitted
// ----------------------
// A DQL query has been submitted and a result set has been produced by the
// database server, even if this set is empty. DQL queries are usually of the
// form SELECT... but there exist DQL queries beginning with other keywords:
// SHOW, DESCRIBE etc. In case of a (successful) DQL query, "sarek" prints 3,
// MySQL errno and MySQL error. For DQL queries the following is true:
//
// THE RESULT SET MUST BE PROCESSED ONE ROW AT A TIME FOR ALL ROWS IN THE
// RESULT SET, OR BE FREED WHILE THERE EXIST UNPROCESSED ROWS REMAINING.
//
// While there exist unprocessed rows in the result set, "sarek" cannot accept
// new queries. The only acceptable input for "sarek" when there exist more
// unprocessed rows in the result set, are the @F and @R directives:
//
//	@F	Ask for the next row of the result set to be returned.
//
//		If there is no active result set, then an error is thrown
//		and "sarek" dies.
//
//		If there are no more rows in the result set, then the
//		result set is freed and an EOD character is returned;
//		EOD means end of data and by default is the 004 character,
//		that is the [Control-D] character.
//
//		Else the row is returned as a tab separated list of values.
//
//	@R	The result set is freed. If there is no active result set,
//		then an error is thrown and "sarek" dies.
//
// There exists another directive to be used for server initialization:
//
//	@S	This directive is followed by tab separated fields:
//
//		user		The user name for database connections.
//
//		password	The password for database connection.
//
//		database	The default database to use.
//
//		host		The database host name (default "localhost").
//
//		charset		The database character set (default "utf8").
//
//		null		The string to be used for null valued fields
//				(default SO control character, ASCII 016
//				octal).
//
//		0/1		For debug mode it must be 1, else 0.
//
// Options
// -------
// Although "sarek" accepts several options, mainly for database connection
// information, most of them are not used in real life, because database
// connection information can be passed to "sarek" via a setup message (@S).
// Setup messages are in the following form:
//
// @Suser<tab>password<tab>database<tab>host<tab>charset<tab>null<tab>debug
//
// Here is a list of the corresponding options:
//
// -u user		user name for database connection.
//
// -p password		password for database connection.
//
// -d database		default database to use.
//
// -h host		database host (default "localhost").
//
// -c charset		database character set.
//
// -n null		string to use for null valued fields representation.
//
// -D			debug mode.
//
// There is one restriction, though: passwords cannot be empty because of AWK
// cannot distinguish between empty (null) strings and undefined variables.
//
// Command line arguments
// ----------------------
// "sarek" can take one command line argument, namely an arbitrary tag in order
// to differentiate different clients spawned by the same awk process. However,
// it's a common practice to set this tag to the level of the "sarek"pawned by
// AWK e.g. 1, 2,...

#include "sarek.h"
#include <unistd.h>
#include <string.h>

static char *set_progname(int, char *[]);
static void usage(void);
static void cleanup(void);

int main(int argc, char *argv[]) {
	int opt;
	int errs = 0;

	argv[0] = (sarek_progname = set_progname(argc, argv));

	while ((opt = getopt(argc, argv, "h:u:p:d:c:n:D")) != -1) {
		switch (opt) {
		case 'u':
			sarek_user = optarg;
			break;

		case 'p':	// empty passwords are not allowed
			sarek_password = optarg;
			break;

		case 'd':
			sarek_database = optarg;
			break;

		case 'h':
			sarek_host = optarg;
			break;

		case 'c':
			sarek_charset = optarg;
			break;

		case 'n':
			sarek_null = optarg;
			break;

		case 'D':
			debug_mode();
			break;

		default:
			errs++;
			break;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc > 1)
	errs++;

	if (errs)
	usage();

	atexit(cleanup);

	// From now on, messages (queries and directives) are being read
	// from the standard input and passed to the "sarek" engine.
	// This is the core loop:

	while (get_message())
	sarek();

	exit(0);
}

void debug_mode(void) {
	sarek_debug = 1;
	sarek_null = "@";
	sarek_eom = ';';
	sarek_sep = '|';
	sarek_eod = '^';
}

static char *set_progname(int argc, char *argv[]) {
	char *p;

	if (argc <= 0) {
		fprintf(stderr, "sarek: invalid command line arguments' count\n");
		exit(EXIT_USAGE);
	}

	if (p = strrchr(argv[0], '/'))
	return p + 1;

	return argv[0];
}

static void usage(void) {
	fprintf(stderr, "usage: %s [OPTIONS] [tag]\n\n"
		"Options\n"
		"-------\n"
		"-u user\n"
		"-p password\n"
		"-d database\n"
		"-h host\n"
		"-c charset\n"
		"-n null\n"
		"-D\n",
		sarek_progname);
	exit(EXIT_USAGE);
}

static void cleanup(void) {
	if (sarek_result) {
		mysql_free_result(sarek_result);
		sarek_result = NULL;
	}

	if (sarek_connection) {
		mysql_close(sarek_connection);
		sarek_connection = NULL;
	}
}
