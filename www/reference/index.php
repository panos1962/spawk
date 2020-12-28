<?php
require "../lib/standard.php";
?>

<!DOCTYPE html>

<html>

<head>
<?php Globals::head(); ?>
<style>
.refitem {
	font-size: 1em;
	margin: 40px 0 0 0;
	padding: 0.4em 0;
	border-style: double none;
	border-width: 3px;
	border-color: #BBBBBB;
}
.refitemMiddle {
	margin: 0 0 0 0;
	border-style: none;
}
.refitemLast {
	margin: 0 0 0 0;
	border-style: none none double none;
}
.refitemFirst {
	border-style: double none none none;
}
.refdef {
	margin: 0.2em 0;
	font-family: monospace;
	white-space: pre-wrap;
}
.chapter {
	margin: 40px 0 0 0;
	border-style: none none solid none;
	border-width: 3px;
	border-color: #BBBBBB;
}
.section {
	margin: 0;
	font-weight: bold;
	font-size: 1em;
	font-style: italic;
	text-decoration: underline;
}
</style>
</head>

<body>

<?php Globals::top("reference"); ?>

<!-- "main" division starts here -->
<div id="main">

<?php Globals::toc_item("top", "SPAWK Reference", 1); ?>

<p>
The SPAWK API consists of a small number of AWK functions and an even smaller
number of AWK global variables.
</p>

<?php Globals::toc_item("spawkAPIfunc", "SPAWK functions", 2, "chapter"); ?>

<?php Globals::toc_item("submit", "spawk_submit() &ndash; Query submission", 3, "refitem"); ?>

<p class="refdef">
spawk_submit(query[, fis])
</p>

<dl>
<dt class="section">Description</dt>

<dd>
<p>
Function <code>spawk_submit()</code> is used from AWK to submit an SQL
query to the database server for execution.
If a database server is already available to accept new queries, then this server
will be used, else a new server is pushed in the server stack for processing the query.
By the way, speaking of servers actually mean database connections.
Queries submitted from AWK, are passed through these connections to the main database
server of the DBMS in use.
</p>
</dd>

<dt class="section">Parameters</dt>

<dd>
<p>
Parameter <code>query</code> is the query to be submitted as a string.
Queries can be of any type, either DQL, DML or DDL. DQL queries produce
result set (even empty), while DML and DQL queries don't produce any result set.
</p>

<p>
Parameter <code>fis</code> is the <i>field indexing scheme</i> for the
result set rows to be returned (if any). There are basically two basic
field indexing schemes, <i>numeric</i> and <i>associative</i>.
Whenever a submitted query produces a result set, then the rows will be returned
to AWK as AWK arrays. Field indexing schemes affect the just the indexing
for the returned array.
</p>

<p>
In <i>numeric</i> indexing schemes the whole row returned is indexed 0
in the array returned. Various fields of the returned row will be indexed
as 1, 2, 3 etc. For example, for the following query:
</p>

<p>
<pre>
SELECT `login`, `registration`, `name`, `email` FROM `user`
</pre>
</p>

<p>
each row will be returned as follows:
row[1] is the login name, row[2] is the registration date,
row[3] is the user name, and row[4] is the email address.
If an <i>associative</i> indexing scheme is used instead,
then row["login"] is the login name, row["registration"]
is the registration name, row["name"] is the user name,
and row["email"] is the email address.
</p>

<p>
The following indexing schemes are valid:
</p>

<dl>
<p>
<dt><strong>0</strong> or <strong>NONE</strong></dt>
<dd>
This is the most simple field indexing scheme where the whole row is returned in the 0 indexed
element of the returned array.
There are no other returned elements in the array,
so whenever the individual field values are not needed, 0 is the recommented fis to use.
</dd>
</p>

<p>
<dt><strong>1</strong> or <strong>NUM</strong> (default)</dt>
<dd>
The whole row is returned in the 0 indexed element, while individual field values will be returned
as 1, 2, 3,… indexed elements of the returned array.
</dd>
</p>

<p>
<dt><strong>2</strong> or <strong>ASSOC</strong></dt>
<dd>
The whole row is returned in the "" indexed element, while individual field values will be returned
as <i>field1</i>, <i>field2</i>, <i>field3</i>,… indexed elements of the returned array,
where <i>field<sub>i</sub></i> are the field names given in the submitted query.
These field names might be unknown at the query submisiion time, as is the case of
<code>SELECT&nbsp;*</code> where the selected field list is not literally specified.
In such cases the 2 fis is recommented, or else one cannot safely map the returned
elements to the correspondig fields.
</dd>
</p>

<p>
<dt><strong>3</strong> or <strong>BOTH</strong></dt>
<dd>
This is the same as the <code>NUM</code> and the <code>ASSOC</code>
indexing schemes together, so there are both numeric AND string indexed elements returned
in the array.
</dd>
</p>
</dl>

<p>
If <i>fis</i> parameter is not specified, then 1 is assumed.
</p>
</dd>
</dl>

<dt class="section">Return value</dt>

<dd>
<p>
Function <code>spawk_submit()</code> returns the following values:
</p>

<dl>
<p>
<dt><strong>1</strong></dt>
<dd>
An error occured, no result set has been produced and the <code>spawk_result</code>
global variable is be set to&nbsp;0.
The server processed the query is ready to accept another query.
</dd>
</p>

<p>
<dt><strong>2</strong></dt>
<dd>
A DML/DDL query has been submitted succesfully.
In that case there is no result set produced and the <code>spawk_result</code>
global variable is set to&nbsp;0.
The server processed the query is ready to accept another query.
</dd>
</p>

<p>
<dt><strong>3</strong></dt>
<dd>
A DQL query has been submitted succesfully.
In that case a result set is produced (even empty) and the <code>spawk_result</code>
global variable is set to&nbsp;1.
The server processed the query cannot accept any new queries until all
rows of the result set are returned to AWK, or freed.
If a new query is submitted while the result set is still active, another server
will be used to process the new query; if there is no such server active, then
a new server will be spawned from AWK and pushed into the <i>server stack</i>
and will stay active until the SPAWK session is alive or the <code>spawk_reset()</code>
is called.
</dd>
</p>
</dl>
</dd>

<dt class="section">Example</dt>

<dd>
<p>
The following scripts:
</p>

<p>
<pre>
spawk_submit("SELECT * FROM `user` WHERE `login` = 'panos')
spawk_fetchrow(user)
</pre>
</p>

<p>
<pre>
spawk_submit("SELECT * FROM `user` WHERE `login` = 'panos', 1)
spawk_fetchrow(user)
</pre>
</p>

<p>
<pre>
spawk_submit("SELECT * FROM `user` WHERE `login` = 'panos', "NUM")
spawk_fetchrow(user)
</pre>
</p>

<p>
will return:
</p>

<p>
<pre class="output">
user[1] = panos
user[2] = 2004-11-30 11:58:44
user[3] = Panos Papadopoulos
user[4] = panos@spawk.info
user[5] = MALE
user[6] = 1962
user[7] = 6367c48dd193d56ea7b0baad25b19455e529f5ee
</pre>
</p>
<p>
while the following scripts:
</p>

<p>
<pre>
spawk_submit("SELECT * FROM `user` WHERE `login` = 'panos', 2)
spawk_fetchrow(user)
</pre>
</p>

<p>
<pre>
spawk_submit("SELECT * FROM `user` WHERE `login` = 'panos', "ASSOC")
spawk_fetchrow(user)
</pre>
</p>

<p>
will return:
</p>

<p>
<pre class="output">
user["login"] = panos
user["email"] = panos@spawk.info
user["password"] = 6367c48dd193d56ea7b0baad25b19455e529f5ee
user["name"] = Panos Papadopoulos
user["registration"] = 2004-11-30 11:58:44
user["gender"] = MALE
user["byear"] = 1962
</pre>
</p>
</dd>
</dl>

<?php Globals::toc_item("fetchrow", "spawk_fetchrow() &ndash; " .
	"Retrieve next row of a result set", 3, "refitem refitemFirst"); ?>

<?php Globals::toc_item("fetchone", "spawk_fetchone() &ndash; " .
	"Retrieve just one row of a result set", 3, "refitem refitemLast"); ?>

<p class="refdef">
spawk_fetchrow(array [, idx])
<br>
spawk_fetchone(array [, idx])
</p>

<dl>
<dt class="section">Description</dt>

<dd>
<p>
Function <code>spawk_fetchrow()</code> retrieves next row of a result set into an array.
The field indexing scheme (fis) of the array is either <i>numeric</i>,
meaning that the individual field values are stored as <code>array[1]</code>,
<code>array[2]</code>,… respectively,
or <i>associative</i>, meaning that the individual field values are stored as
<code>array[FIELD1]</code>, <code>array[FIELD2]</code>,… respectively,
or <i>both</i>, meaning that both of the above indexing schemes will be used.
The field indexing scheme is specified in the query submission via
<code>spawk_submit()</code> function (default <i>numeric</i>).
</p>

<p>
Calling <code>spawk_fetchrow()</code> when no result set exists is a fatal error.
All rows of the result set must be retrieved in order to free current client
to accept new queries.
If no more rows are needed from the result set, the set may be freed by calling the
<code>spawk_freerest()</code> function.
</p>

<p>
Function <code>spawk_fetchone()</code> is just the same as <code>spawk_fetchrow()</code>,
but frees the result set after the row retrieval.
</p>
</dd>

<dt class="section">Parameters</dt>

<dd>
<p>
Function <code>spawk_fetchrow()</code> accepts at least one parameter,
namely the name of the array where the next row of the result set will be retrieved.
</p>

<p>
The second parameter is the whole row index.
If not given, then no whole row element is returned,
except for 0 valued <code>fis</code>, when the whole row is returned indexed by 0.
</p>
</dd>

<dt class="section">Return value</dt>

<dd>
<p>
The number of fields of the retrieved row is returned except of field indexing
scheme 0 where 1 is returned.
If there are no more rows in the result set, 0 is returned.
</p>
</dd>

<?php $example = 1; ?>

<dt class="section">Example (<?php print $example++; ?>)</dt>

<dd>
<p>
<pre>
if (!spawk_submit("SELECT `login`, `name`, `email` FROM `user`"))
return

while (spawk_fetchrow(user))
print "login:", user[1], "name:", user[2], "email:", user[3]
</pre>
</p>
</dd>

<dt class="section">Example (<?php print $example++; ?>)</dt>

<dd>
<p>
<pre>
if (!spawk_submit("SELECT * FROM `user`"), 2)
return

while (spawk_fetchrow(user))
print "login:", user["login"], "name:", user["name"], "email:", user["email"]
</pre>
</p>
</dd>
</dl>

<?php Globals::toc_item("freerest", "spawk_freerest() &ndash; Free result set", 3, "refitem"); ?>

<p class="refdef">
spawk_freerest()
</p>

<dl>
<dt class="section">Description</dt>

<dd>
<p>
Function <code>spawk_freerest()</code> frees the result set and releases
current server to accept new queries.
</p>
</dd>

<dt class="section">Return value</dt>

<dd>
<p>
Function <code>spawk_freerest()</code> returns no value.
</p>
</dd>
</dl>

<?php Globals::toc_item("reset", "spawk_reset() &ndash; Reset server stack", 3, "refitem"); ?>

<p class="refdef">
spawk_reset()
</p>

<dl>
<dt class="section">Description</dt>

<dd>
<p>
Function <code>spawk_reset()</code> closes all servers spawned by AWK and resets the server stack.
</p>
</dd>

<dt class="section">Return value</dt>

<dd>
<p>
Function <code>spawk_reset()</code> returns the number of servers closed.
</p>
</dd>
</dl>

<?php Globals::toc_item("escape", "spawk_escape() &ndash; Escape quotes", 3, "refitem"); ?>

<p class="refdef">
spawk_escape(string[, unenclosed])
</p>

<dl>
<dt class="section">Description</dt>

<dd>
<p>
Function <code>spawk_escape()</code> accepts a string argument and returns the
string with single quotes escaped.
If <code>unenclosed</code> is true, the string is returned with escaped single quotes,
but without enclosing quotes.
</p>
</dd>

<dl>
<dt class="section">Return value</dt>

<dd>
<p>
Function <code>spawk_escape()</code> returns the string with single quotes escaped.
The returned string is enclosed in single quotes so it can be used in queries,
unless <code>unenclosed</code> non-zero parameter is passed.
</p>
</dd>

<dt class="section">Example</dt>

<dd>
<p>
<pre>
login = "Smith's"
spawk_submit("SELECT * FROM `user` WHERE `login` = " spawk_escape(login)")
</pre>
</p>

<p>
is equivalent to
</p>

<p>
<pre>
spawk_submit("SELECT * FROM `user` WHERE `login` = 'Smith\'s'")
</pre>
</p>
</dd>
</dl>

<?php Globals::toc_item("getpass", "spawk_getpass() &ndash; Prompt for password", 3, "refitem"); ?>

<p class="refdef">
spawk_getpass([prompt])
</p>

<dl>
<dt class="section">Description</dt>

<dd>
<p>
Function <code>spawk_getpass()</code> causes AWK to pause and prompt for a password
from the control terminal.
Usually needed for database root access, but can be used for normal users too.
</p>
</dd>

<dt class="section">Return values</dt>

<dd>
<p>
Function <code>spawk_getpass()</code> returns the specified password.
</p>
</dd>

<dt class="section">Example</dt>

<dd>
<p>
<pre>
spawk_sesami("root")
spawk_password = spawk_getpass("Enter root password: ")
</pre>
</p>

<p>
The same could be achieved by:
</p>

<p>
<pre>
spawk_sesami("root", spawk_getpass("Enter root password: "))
</pre>
</p>
</dd>
</dl>

<?php Globals::toc_item("spawkAPIvar", "SPAWK global variables", 2, "chapter"); ?>

<p>
Besides the functions, the SPAWK API contains a couple of AWK variables.
Some of them are intrisic and it's an error to modify their value by hand,
such as <code>spawk_sqlerrno</code>, <code>spawk_affected</code> etc,
but most of them are set by the user and controls many of the SPAWK functions,
e.g. in order to access a MySQL/MariaDB database, specific <code>spawk_sesami</code>
array elements must previously be set.
</p>

<?php Globals::toc_item("sesami", "spawk_sesami &ndash; Database authentication", 3, "refitem"); ?>

<p class="refdef">
spawk_sesami["dbuser"]
spawk_sesami["dbpassword"]
spawk_sesami["dbname"]
spawk_sesami["dbcharset"]
</p>

<dl>
<dt class="section">Description</dt>

<dd>
<p>
Array <code>spawk_sesami</code> is used to provide AWK with the necessary
database connection information that will be used whenever a new server is spawned from AWK.
If any of the array elements are missing, then the usual MySQL/MariaDB
authentication methods will be used (<code>cnf</code> files etc).
Usually you set these elements once in the <code>BEGIN</code> section
of your AWK script, but you can change the connection parameters
in the middle of the SPAWK session; doing so the already spawned servers
will keep accepting requests, but any new server pushed in the stack will be
connected with the new connection information.
</p>

<p class="warning warningBlue">
<strong>Notice!</strong>
<br>
If you want to close all active servers use <code>spawk_reset()</code> function.
</p>
</dd>

<dt class="section">Valid indices</dt>

<dd>
<dl>
<dt><code><strong>dbuser</strong></code></dt>
<dd>
<p>
The database user to be used for the connection with the database server.
</p>
</dd>

<dt><code><strong>dbpassword</strong></code></dt>
<dd>
<p>
The password of the database user to be used for the connection with the database server.
</p>
</dd>

<dt><code><strong>dbname</strong></code></dt>
<dd>
<p>
The default database to use.
</p>
</dd>

<dt><code><strong>dbhost</strong></code></dt>
<dd>
<p>
The default host machine for the database server.
</p>

<dt><code><strong>dbcharset</strong></code></dt>
<dd>
<p>
The default character set to be used for the database connection, e.g. "<code>utf8</code>".
</p>
</dd>
</dl>
</dd>

<dt class="section">Example</dt>

<dd>
<p>
<pre>
@load "spawk.awk"

BEGIN {
	spawk_sesami["dbuser"] = "chat"
	spawk_sesami["dbpassword"] = "xxx"
	spawk_sesami["dbname"] = "chat"
	...
</pre>
</p>

<p>
The above SPAWK snippet sets <code>chat</code> user with <code>xxx</code>
password to be used for connecting to the <code>chat</code> database.
The connections will be established whenever a new server is spawned
from AWK.
</p>
</dd>
</dl>

<?php Globals::toc_item("spawkOFS", "spawk_OFS &ndash; Field separator", 3, "refitem"); ?>

<p class="refdef">
spawk_OFS
</p>

<dl>
<dt class="section">Description</dt>

<dd>
<p>
By default selected fields are separated by <code>OFS</code>,
but <code>spawk_OFS</code> variable can be used to set another separator
for selected fields.
</p>
</dd>

<dt class="section">Default value</dt>

<dd>
<p>
If neither <code>spawk_OFS</code> nor <code>OFS</code> are set, then the
tab character is used.
</p>
</dd>
</dl>

<?php Globals::toc_item("spawknull", "spawk_null &ndash; Null value string", 3, "refitem"); ?>

<p class="refdef">
spawk_null
</p>

<dl>
<dt class="section">Description</dt>

<dd>
<p>
<code>spawk_null</code> variable is a string used for null database valued fields.
Null valued fields are not the same as empty or zero valued fields.
The user can change <code>spawk_null</code> value either in the <code>BEGIN</code>
section of the AWK script, or using <code>-v</code> command line option.
</p>
</dd>

<dt class="section">Default value</dt>

<dd>
<p>
Control-N (ASCII 016 octal)
</p>
</dd>
</dl>

<?php Globals::toc_item("sqlerror", "spawk_sqlerrno, spawk_sqlerror &ndash; SQL error codes/messages", 3, "refitem"); ?>

<p class="refdef">
spawk_sqlerrno
spawk_sqlerror
</p>

<dl>
<dt class="section">Description</dt>

<dd>
<p>
<code>spawk_sqlerrno</code> and <code>spawk_sqlerror</code> are set to the
corresponding values after query failure.
These values are not printed by default, except if <code>spawk_verbose</code> flag is
set to a non-zero value.
</p>
</dd>

<dt class="section">Example</dt>

<dd>
<p>
<pre>
@load "spawk.awk"

...
if (spawk_submit(query) == 1) {
	print spawk_sqlerrno, spawk_sqlerror >"/dev/stderr"
	return
}
...

</pre>
</p>
</dd>
</dl>

<?php Globals::toc_item("spawkverbose", "spawk_verbose &ndash; Verbose mode flag", 3, "refitem"); ?>

<p class="refdef">
spawk_verbose
</p>

<dl>
<dt class="section">Description</dt>

<dd>
<p>
If set to non-zero value, sets SPAWK to verbose mode.
In verbose mode various messages are printed while SPAWK program is running.
</p>
</dd>
</dl>

<?php Globals::toc_item("spawkaffected", "spawk_affected &ndash; Affected rows count", 3, "refitem"); ?>

<p class="refdef">
spawk_affected
</p>

<dl>
<dt class="section">Description</dt>

<dd>
<p>
After submitting DML query, affected rows count is stored in <code>spawk_affected</code>
global variable.
</p>
</dd>
</dl>

<?php Globals::toc_item("spawkinsertid", "spawk_insertid &ndash; Autoincrement inserted row ID", 3, "refitem"); ?>

<p class="refdef">
spawk_insertid
</p>

<dl>
<dt class="section">Description</dt>

<dd>
<p>
After inserting new row with autoincrement primary key column, the generated ID of the
newly inserted row is stored in <code>spawk_insertid</code> global variable.
</p>
</dd>
</dl>

<?php Globals::toc_item("spawkmaxconn", "spawk_maxconn &ndash; Maximum number of servers", 3, "refitem"); ?>

<p class="refdef">
spawk_maxconn
</p>

<dl>
<dt class="section">Description</dt>

<dd>
<p>
Is the maximum number of database servers (connections) allowed for SPAWK.
Default value is 10 and maximum value is 100.
Can be set only from the command line.
</p>
</dd>

<dt class="section">Example</dt>

<dd>
<p>
<pre>
awk -v spawk_maxconn=20 -f test.awk
</pre>
</p>
</dd>
</dl>

</div>
<!-- end of "main" division -->

<?php Globals::bottom(date("M d, Y", filemtime(__FILE__))); ?>

</body>

</html>
