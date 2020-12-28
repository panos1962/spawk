##############################################################################
#
# spawk.awk - SPAWK API
#
# Copyright (C) 2004-2017, by Panos Papadopoulos
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Name: Panos Papadopoulos
# Email: panos1962_AT_gmail_DOT_com
#
##############################################################################

# Passing empty values causes "sarek" to use other authentication
# methods. This may become a problem in case of empty password.
# To solve this problem we must use the "spawk_empty_password"
# special value for empty password, e.g.
#
#	spawk_sesami("panos")
#
# will cause "sarek" to search the MySQL/MariaDB configuration files
# for the "panos" user authentication. But what if the user password
# is empty?
#
#	spawk_sesami("panos", spawk_empty_password)

function spawk_sesami(user, password, database, host,		version, n, v) {
	spawk_user = user
	spawk_password = password
	spawk_database = database
	spawk_host = host
}

# Function:
# spawk_submit(query [, fis])
#
# Parameters:
# query: An SQL query to be passed to database server for execution.
# fis: It's the field indexing scheme, that is one of 0, 1, 2 and 3.
#
# fis and query are passed to sarek server. Sarek passes the query
# to the database server and returns the results to awk. Awk puts
# each result row in an an array indexed according to the given fis.
#
# Return values:
# 1: Error
# 2: DDL/DML
# 3: DQL
#
# Also reset and set:
# spawk_sqlerrno: mysql_errno (number)
# spawk_sqlerror: mysql_error (message string)
#
# spawk_affected: affected records (number)
# spawk_insertid: last insert id (number)
# spawk_info: massive updates info (message string)
#
# spawk_result: 0 for DML/DDL, 1 for DQL 

function spawk_submit(query, fis,		n, response, i) {
	spawk_logwrite("SQL: " query)

	if (!((fis "") in spawk_fis))
	spawk_fatal(fis ": invalid field indexing scheme")

	spawk_logwrite("FIS: " fis)
	fis = spawk_fis[fis]

	spawk_sqlerrno = 0
	spawk_sqlerror = ""

	spawk_affected = 0
	spawk_insertid = 0
	spawk_info = ""

	# By default query is considered to produce no result set.

	spawk_result = 0

	# Empty queries are allowed and considered as DML but do nothing.
	# No need to push a new server.

	if (query == "")
	return 2

	# Push a new server into the stack and pass fis and query to this
	# new server as a message, e.g. given the following:
	#
	# fis: "ASSOC"
	# query: "SELECT * FROM `user`"
	#
	# the message passed to the new server is: "2select * FROM `user`"
	# as of "2" is the fis code for "ASSOC" fis.

	spawk_server_push()
	spawk_server_put(fis query)

	# After passing the query message to the new server, we get the
	# response which must have at least one field, namely the answer
	# code.
	#
	# Response fields
	# ===============================================================
	# 1 (Error)
	# MySQL errno (numeric)
	# MySQL error (string)
	# ===============================================================
	# 2 (DML/DDL query)
	# affected rows (numeric)
	# last insert ID (numeric)
	# info (string)
	# ================================================================
	# 3 (DQL query)
	# field indexing scheme (fis):
	#	0: x[0] = row
	#	1: x[0] = row, x[1] = field1, x[2] = field2,...
	#	2: x[""] = row, x["col1"] = field1, x["col2"] = field2,...
	#	3: both of 1 and 2
	# [field1]
	# [field2]
	# ...
	# ================================================================

	if ((n = spawk_server_get(response, 1)) < 1)
	spawk_fatal("deficient sarek response (metadata)")

	# Failed SQL query

	if (response[1] == 1) {
		if (n != 3)
		spawk_fatal("invalid ERROR sarek response (metadata)")

		spawk_sqlerrno = response[2] + 0
		spawk_sqlerror = response[3]

		if (spawk_verbose && (spawk_sqlerrno || spawk_sqlerror))
		spawk_error("MySQL: (" spawk_sqlerrno "): " spawk_sqlerror)

		spawk_server_pop()
		return 1
	}

	# DML/DDL (no result set)

	if (response[1] == 2) {
		if (n != 4)
		spawk_fatal("invalid DML/DDL sarek response (metadata)")

		spawk_affected = response[2] + 0
		spawk_insertid = response[3] + 0
		spawk_info = response[4] + 0

		spawk_server_pop()
		return 2
	}

	if (response[1] != 3)
	spawk_fatal("invalid sarek response (query)")

	# SQL/DQL (result set has been produced, even if empty).
	# Second field is the field indexing scheme (fis):
	# 0:	row[0] = ROW
	# 1:	row[0] = ROW, row[1] = F1 row[2] = F2,...
	# 2:	row[""] = ROW, row[C1] = F1, ROW[C2] = F2,...
	# 3:	both 1 and 2

	if (n < 2)
	spawk_fatal("invalid DQL sarek response (metadata)")

	# Set the fis for the current server to the "spawk_field[0]"
	# Field names are stored to "spawk_field[1]", "spawk_field[2]" etc

	delete spawk_field[spawk_level]
	if ((spawk_field[spawk_level][0] = response[2] + 0) > 1) {
		for (i = 3; i <= n; i++)
		spawk_field[spawk_level][i - 2] = response[i]
	}

	# Set "spawk_result" flag to true, so one can check if the
	# query produced result set (even an empty one).

	spawk_result = 1

	# In case of a successful DQL query, we do not pop the server
	# until all of the rows of the result set are returned to awk,
	# or the result set is relased.

	return 3
}

function spawk_fetchrow(row,			fis, n, x, i) {
	spawk_server_put(spawk_message["FETCHROW"])

	# When there are no more rows remained in the result set,
	# we pop the server off the stack. There is no need to
	# free the result set literally, because this has been
	# already done by the server on the last (unfulfiled)
	# row request.

	if (!spawk_server_get(row)) {
		spawk_server_pop()
		return 0
	}

	# row[0] contains the row as a whole as readed by the
	# "spawk_server_get" function call above.

	spawk_logwrite(row[0])

	# fis 0 is the simplest of all field indexing schemes
	# where the whole returned ROW is returned in row[0]
	# and nothing else is returned.

	if ((fis = spawk_field[spawk_level][0]) == 0)
	return 1

	x = row[0]
	n = split(x, row, spawk_tab)

	# In case there is just one field in the row returned,
	# split returns 0, so this must be set to 1.

	if (!n)
	n = 1

	# For NUM field indexing schemes we have to set row[0]
	# to the whole ROW returned.

	if ((fis == 1) || (fis == 3))
	row[0] = x

	# fis 1 means that the whole ROW is returned in row[0],
	# FIELD1 in row[1], FIELD2 in row[2] etc. This has
	# already be done by the above split.

	if (fis == 1)
	return n

	# fis 2 and 3 are ASSOC field indexing schemes. However
	# do not be in a hurry to set row[""] to the whole ROW
	# returned!

	for (i = 1; i <= n; i++) {
		row[spawk_field[spawk_level][i]] = row[i]

		# fis 2 is just ASSOC not NUM

		if (fis == 2)
		delete row[i]
	}

	# Keep the following line here. Do not move it before
	# the above loop, as you might loose the index due to
	# possible missing field names.

	row[""] = x

	return n
}

function spawk_fetchone(row,			n) {
	if (n = spawk_fetchrow(row))
	spawk_freerest()

	return n
}

function spawk_freerest() {
	spawk_server_put(spawk_message["FREEREST"])
	spawk_server_pop()
}

function spawk_escape(x, noquotes) {
	gsub(/'/, "\\'", x)
	return noquotes ? x : "'" x "'"
}

function spawk_is_null(x) {
	return (x == spawk_null)
}

function spawk_error(msg) {
	print "SPAWK: " msg >"/dev/stderr"
}

function spawk_fatal(msg, err) {
	spawk_error(msg)
	exit(err + 0)
}

################################################################################

BEGIN {
	spawk_eom = spawk_debug ? ";" : "\004"
	spawk_tab = spawk_debug ? "|" : "\011"
	spawk_eod = spawk_debug ? "^" : "\004"
	spawk_empty_password = "d0d148e9af51b22184758e53fe4d3e33a6e44fa9"

	if (!spawk_null)
	spawk_null = spawk_debug ? "@" : "\016"

	if (!spawk_host)
	spawk_host = "localhost"

	if (!spawk_charset)
	spawk_charset = "utf8"

	if (!spawk_sarek)
	spawk_sarek = "sarek"

	spawk_verbose = 1
	spawk_level = 0

	spawk_fis[0] = 0
	spawk_fis[1] = 1
	spawk_fis[2] = 2
	spawk_fis[3] = 3
	spawk_fis[""] = 1
	spawk_fis["NONE"] = 0
	spawk_fis["NUM"] = 1
	spawk_fis["ASSOC"] = 2
	spawk_fis["BOTH"] = 3

	spawk_message["SETUP"] = "@S"
	spawk_message["GETPASS"] = "@P"
	spawk_message["FETCHROW"] = "@F"
	spawk_message["FREEREST"] = "@R"
}

function spawk_server_push(			server) {
	if (spawk_level > 99)
	spawk_fatal("too many servers pushed")

	spawk_level++

	spawk_data[spawk_level] = 1

	delete spawk_field[spawk_level]
	spawk_field[spawk_level][0] = 1

	server = spawk_current_server()

	if (server)
	return server

	spawk_logwrite("Creating SPAWK database server " spawk_level)

	server = "exec " (spawk_sarek ? spawk_sarek : "sarek") " " spawk_level
	printf("%s%s\t%s\t%s\t%s\t%s\t%s\t%d\004\n", spawk_message["SETUP"], \
		spawk_user, spawk_password, spawk_database, spawk_host, \
		spawk_charset, spawk_null, spawk_debug) |& server
	spawk_stack[spawk_level] = server
	return server
}

function spawk_server_pop() {
	spawk_data[spawk_level--] = 0
}

function spawk_current_server() {
	if (spawk_level <= 0)
	spawk_fatal("no active database server")

	return spawk_stack[spawk_level]
}

function spawk_reset(			i, n) {
	for (i in spawk_stack) {
		if (spawk_data[i])
		print spawk_message["FREEREST"] |& spawk_stack[i]

		close(spawk_stack[i])
		n++
	}

	delete spawk_stack
	delete spawk_data
	delete spawk_field
	spawk_level = 0

	return n + 0
}

function spawk_server_put(query) {
	print query spawk_eom |& spawk_current_server()
}

function spawk_server_get(response, splt,		ret, x, n, i) {
	delete response

	if (!spawk_data[spawk_level])
	spawk_fatal("no active server")

	ret = (spawk_current_server() |& getline x)

	if (ret == -1)
	spawk_fatal("I/O error")

	if (ret == 0)
	spawk_fatal("data shortage")

	if (x == spawk_eod)
	return 0

	if (splt)
	n = split(x, response, spawk_tab)

	response[0] = x
	return n ? n : 1
}

function spawk_logwrite(x,			i) {
	if (!spawk_logfile)
	return

	for (i = 0; i < spawk_level; i++)
	printf "\t" >>spawk_logfile

	print x >>spawk_logfile
}

# To prompt for a password send a "password" message to an ad hoc server.
# Then read back the password given and close the server.

function spawk_getpass(prompt,		password) {
	print spawk_message["GETPASS"] prompt spawk_eom |& spawk_sarek
	spawk_sarek |& getline password
	close(spawk_sarek)
	return password
}
