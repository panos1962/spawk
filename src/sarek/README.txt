sarek - SPAWK's dedicated database server
=========================================

Spock is a fictional character in the Star Trek media franchise.
SPAWK is the SQL Powered AWK, that is AWK with SQL database access ability.
In order for AWK to communicate with MySQL databases via SQL queries, an AWK library
is used along with a custom MySQL database client. This client is a pure executable
named "sarek"; Sarek and Amanda were Spock's parents.
So, sarek is considered to be a database server for SPAWK, while it's a client
program for the MySQL/MariaDB database.

Quick reference
---------------

Use "spawk_sesami" to specify the database credentials.

Use "spawk_submit" for submitting SQL queries.

For DQL queries, that is queries that produce result rows, use "spawk_fetchrow"
to fetch the resulting rows one by one, or "spawk_fetchone" to fetch the first
row and release the server to accept another query.

For DML/DDL queries "spawk_affected" variable contains the number of the rows
affected, "spawk_insertid" contains the last inserted ID for auto increment
primary key values and, finally, "spawk_info" contains information message
concerning massive updates.

Null values are displayed as empty strings. Use "spawk_null" variable to specify
another string for null values, e.g.

	awk -v spawk_null="\016" -f test1.awk

will cause null valued columns to be displayed as SO [Control-N] characters.
