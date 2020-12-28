<?php
require "lib/standard.php";
?>

<!DOCTYPE html>

<html>

<head>
<?php Globals::head(); ?>
</head>

<body>

<?php Globals::top("home"); ?>

<!-- "main" division starts here -->
<div id="main">

<?php Globals::toc_item("top", "SPAWK - SQL Powered AWK", 1); ?>

<p>
AWK is a convenient and expressive programming language that can be
used in a wide variety of computing and data manipulation tasks.
MySQL is an open-source relational database management system (RDBMS),
while MariaDB is a community-developed fork of the MySQL RDBMS,
created after Oracle corporation acquired Sun Micorsystems (and MySQL) at 2008.
MariaDB intends to maintain high compatibility with MySQL,
ensuring a drop-in replacement capability with library binary equivalency
and exact matching with MySQL APIs and commands.
SPAWK enables AWK to submit SQL queries to MySQL/MariaDB
relational databases; query results are returned back to AWK
for further processing.
</p>

<?php Globals::toc_item("introduction", "Introduction to AWK", 2); ?>

<p>
AWK is a well known programmable pattern matching and data manipulation
software tool found in every UNIX-like operating system.
Every AWK program is in the form of:
</p>

<p>
<pre>
pattern { action }
pattern { action }
...
</pre>
</p>

<p>
AWK reads textual input on a line by line basis.
Each line is checked against every one of the given <i>patterns</i>
and whenever a line matches a pattern, the corresponding action is taken.
The keyword <code><strong>next</strong></code> can be used in an action to skip the remaining patterns.
There is a special pattern named <code><strong>BEGIN</strong></code> with an action to be
taken before any input line has been read.
Another special pattern named <code><strong>END</strong></code> can be used for actions to be taken
after all input lines has been read.
</p>

<p>
AWK splits each input line in <i>fields</i>. Every single word of the input
line is considered as a field, but that can be altered using the special
variable <code><strong>FS</strong></code> (field separator).
The total number of fields in a line is stored in the <code><strong>NF</strong></code>
variable, while the fields can be accessed through the <code><strong>$1</strong></code>,
<code><strong>$2</strong></code>… special variables. The whole input line itself can be
accessed through the special variable <code><strong>$0</strong></code>.
</p>

<p>
Given a list of integer numbers, the following program will count the
multiples of 2, 3, 5 and 7:
</p>

<p>
<pre>
BEGIN { mul2 = 0; mul3 = 0; mul5 = 0; mul7 = 0 }
($0 % 2) == 0 { mul2++ }
($0 % 3) == 0 { mul3++ }
($0 % 5) == 0 { mul5++ }
($0 % 7) == 0 { mul7++ }
END { print mul2, mul3, mul5, mul7 }
</pre>
</p>

<p>
Given the input number 8 only one action is taken, because the remainder
of the division of 8  by 2 is zero, while the divisions of 8 by 3, 5 and 7 are non-zero.
However, for the input number 42 three actions are taken because the remainder
of division of 42 by 2, 3 and 7 are zero.
For the input number 97 no action is taken because 97 is a prime number.
</p>

<?php Globals::toc_item("vanillaSQL", "Using SQL to produce primary data", 2); ?>

<p>
Without using SPAWK, the only way to process data stored in a MySQL/MariaDB database
with AWK is to extract the desired data from the database and then pass the
extracted data to AWK.
Assume that we have a <i>chat</i> website based on a MySQL/MariaDB database.
There exists a <i>user</i> table in the database, holding a record for each user
registered user.
There also exists a <i>relation</i> table holding the relations between users,
that is users marked as <i>friends</i> or <i>blocked</i> by other users.
</p>

<p>
We want to produce a report with all registered users, along with the count of
friends and blocked users for every one of them.
The report must be sorted by login names.
We may use plain vanilla SQL to produce such a report, but maybe it's not so easy to
do so.
In order to avoid <code>COUNT</code> syntax complexities which may lead to
erroneus results, we decide to use a couple of SQL scripts to extract the desired
data from the database and then pass the results to AWK to produce the final report.
First thing to do is to produce a list of all registered users,
even those users that are not yet related with other users:
</p>

<p>
<pre>
SELECT `login` FROM `user`;
</pre>
</p>

<p>
Use the <code>mysql</code> standard client program to run the above query and
store the result rows in a file.
Each row consists of just one column, namely the login name of the user:
</p>

<p>
<pre class="output">
panos
maria
smith
chris
arnold
brian
alfred
peter
...
</pre>
</p>

Next thing to do is to produce relations' counts for all users
having related users:
</p>

<p>
<pre>
SELECT `user`, `relationship`, COUNT(*) FROM `relation` GROUP BY `user`, `relationship`;
</pre>
</p>

<p>
The output of the above query contains three columns, namely the login name,
the relation kind (FRIEND or BLOCKED) and the corresponding count:
</p>

<p>
<pre class="output">
alfred		FRIEND		22
alfred		BLOCKED		13
arnold		BLOCKED		 3
brian		FRIEND		18
brian		BLOCKED		 2
maria		FRIEND		 6
panos		FRIEND		10
panos		BLOCKED		 7
...
</pre>
</p>

<p>
We can now sort all of the above output and pass the sorted data to AWK.
Lines read by AWK will have either one field (output from the first query),
or three fields (output from the second query):
</p>

<p>
<pre class="output">
<b>alfred</b>
alfred		FRIEND		22
alfred		BLOCKED		13
<b>arnold</b>
arnold		BLOCKED		 3
<b>brian</b>
brian		FRIEND		18
brian		BLOCKED		 2
<b>chris</b>
<b>maria</b>
maria		FRIEND		 6
<b>panos</b>
panos		FRIEND		10
panos		BLOCKED		 7
...
</pre>
</p>

<?php Globals::toc_item("vanillaAWK", "Using AWK to process primary data", 2); ?>

<p>
The fact is that because the data are sorted, there exists one line with the
user login name for all registered users and after each name may follow
at most two lines with relations' counts.
To form the desired report we pass the sorted data to AWK:
</p>

<p>
<pre>
NF == 1 {
	# New user encountered. Print previous user data.

	if ($1 != user)
	print_user()

	# Now keep new user name in mind and reset counters.

	user = $1
	delete count

	next
}

# This line comes from the second SQL query. First column is
# the user name, second column is the relationship (FRIEND or
# BLOCKED) and third column is the the relevant count.

NF == 3 {
	count[$2] = $3
	next
}

{
	print $0 ": syntax error" &gt;"/dev/stderr"
}

# After all input has been read, the last user statistics must
# be printed.

END {
	print_user()
}

function print_user() {
	if (user)
	print user, count["FRIEND"] + 0, count["BLOCKED"] + 0
}
</pre>
</p>

<p>
The output of the above AWK script will be:
</p>

<p>
<pre class="output">
alfred		22	13
arnold		 0	 3
brian		18	 2
chris		 0	 0
maria		 6	 0
panos		10	 7
...
</pre>
</p>

<p>
Assuming that the above scripts were stored as <code>relcnt1.sql</code>,
<code>relcnt2.sql</code> and <code>relcnt.awk</code>, we can run
the following one-liner to produce the desired report:
</p>

<p>
<pre>
sort &lt;$(mysql &lt;relcnt1.sql) &lt;$(mysql &lt;relcnt2.sql) | awk -f relcnt.awk
</pre>
</p>

<p>
Of course, <code>mysql</code> must run with the appropriate options for
user authentication and to produce raw results free of any kind of headers.
</p>

<?php Globals::toc_item("SPAWK", "Using SPAWK for elegance and simplicity", 2); ?>

<p>
This was a pretty cumbershome and tedious process to carry out a simple report.
Moreover, all of the above lack any elegance at all and this is usually a sign
of bad software engineering.
But there is no reason to dispair, here comes SPAWK to our rescue:
</p>

<p>
<pre>
@load "spawk"

BEGIN {
	spawk_sesami["dbuser"] = "chat"
	spawk_sesami["dbpassword"] = "xxx"
	spawk_sesami["dbname"] = "chat"

	spawk_submit("SELECT `login` FROM `user` ORDER BY `login`")

	while (spawk_fetchrow(user))
	process_user(user[1])
}

function process_user(login,		relation, count) {
	spawk_submit("SELECT `relationship` FROM `relation` WHERE `user` = " spawk_escape(user[1]))

	while (spawk_fetchrow(relation))
	count[relation[1]]++

	print user[1], count["FRIEND"] + 0, count["BLOCKED"] + 0
}
</pre>
</p>

<p>
In the <code>BEGIN</code> section we provide the credentials for SPAWK
to open the <i>chat</i> database and then submit the main SQL query to select
the login names of all the registered users in the desired order.
For each user selected we submit another SQL query to select the user's relations.
For each selected relation we increase the count of the correspondig relationship
(<i>FRIEND</i> or <i>BLOCKED</i>).
After processing all the relations for each user, we print the user's login name
and the relations' counts of interest.
</p>

<?php Globals::toc_item("spawkAPI", "Loading the SPAWK dynamic extension", 2); ?>

<p>
In order to run SQL from within AWK scripts, AWK must be supplied with an API of
AWK functions. This API is the <code>spawk.so</code> dynamic extension library and it's just a collection
of less than ten AWK functions, along with some gobal variables, all of them named
as <code>spawk_</code><i>something</i>, e.g. <code>spawk_submit()</code> (function),
<code>spawk_fetchrow()</code> (function), <code>spawk_sesami</code> (array),
<code>spawk_verbose</code> (variable) etc.
</p>

<p>
In order for AWK to be equipped with the SPAWK API functions and variables,
the <code>spawk.so</code> shared library must be loaded.
This can be achieved either by including the <code>spawk.so</code>
shared library in the command line:
</p>

<p>
<pre>
awk --load spawk -f relcnt.awk
</pre>
</p>

<p>
or by loading the <code>spawk.so</code> shared library inside the main AWK script using the
<code>@load</code> directive:
</p>

<p>
<pre>
@load "spawk"

BEGIN {
...
</pre>
</p>

<p>
In order to avoid using full pathnames it's advisable to set <code>AWKLIBPATH</code>
accordingly, e.g. by adding a line in the <code>/etc/environment</code> file:
</p>

<p>
<pre>
export AWKLIBPATH="/home/panos/lib/gawk:/usr/local/lib/gawk"
</pre>
</p>

<p>
It's even better for <code>spawk.so</code> file to be located in the
<code>/usr/local/lib/gawk</code> directory because this directory is included
to the default <code>AWKLIBPATH</code>, so there's no need to set or change anything;
however, you may need administrative permissions to locate the <code>spawk.so</code>
file in the <code>/usr/local/lib/gawk</code> directory.
</p>

</div>
<!-- end of "main" division -->

<?php Globals::bottom(date("M d, Y", filemtime(__FILE__))); ?>

</body>

</html>
