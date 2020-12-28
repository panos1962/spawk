<?php
require "../lib/standard.php";
?>

<!DOCTYPE html>

<html>

<head>
<?php Globals::head(); ?>
</head>

<body>

<?php Globals::top("tutorial"); ?>

<!-- "main" division starts here -->
<div id="main">

<?php Globals::toc_item("top", "SPAWK Tutorial", 1); ?>

<p>
In this page we setup a MySQL/MariaDB test database and use SPAWK to
access this database with AWK.
You can download most of the files and scripts found in this page and run everything locally,
though you must be careful in order not to damage your databases or other files or scripts.
Have fun!
</p>

<?php Globals::toc_item("download", "Download and install SPAWK", 2); ?>

<p>
In order to run this tutorial, SPAWK must be downloaded and installed locally.
After downloading SPAWK tarball, you may have to download the MySQL/MariaDB
<code>libmysqlclient.so</code> dynamic library and
<code>mysql_config</code> utility program in order to compile and link the
<code>spawk</code> gawk dynamic extension.
Refer to the [&nbsp;<a href="../download" target="download">Download</a>&nbsp;]
page for further instructions on how to download and install SPAWK to your
local system.
</p>

<p class="warning warningYellow">
<strong>Warning!</strong>
<br>
Do not remove the <code>spawk</code> directory structure extracted from the SPAWK tarball
after installation, because some files will be needed for this tutorial
course to complete.
After fininsihing this tutorial course the files will no longer be needed,
so you can safely remove the whole <code>spawk</code> directory structure.
</p>


<?php Globals::toc_item("chat", "The <i>chat</i> application", 2); ?>

<p>
This tutorial page is about a hypothetical <i>chat</i> application,
where users all over the world can register and start chatting to each other.
Users can setup relationships to other users marking them as <i>friends</i>
or <i>blocking</i> them out.
Users can also send off-line messages to each other, forming something
like an internal email system inside the <i>chat</i> application.
Later on we may add <i>rooms</i>, <i>dates</i>, or other useful objects
to the <i>chat</i> application.
</p>

<p>
Our <i>chat</i> application consists of the usual major components such as
web server, database, server side programs, client side programs etc.
In this tutorial we cope mainly with the database and some server side SPAWK programming.
We'll show you how to use SPAWK effectiveley for a wide range of database operations,
from creating and populating the database, to producing useful database reports and statistics.
</p>

<?php Globals::toc_item("database", "The <i>chat</i> database", 2); ?>

<p>
The whole <i>chat</i> application lies on a MySQL/MariaDB database, holding
users, user relations, messages etc.
The schema of the database is by no means complicated; there exist less than five tables
with a total of no more than twenty columns.
</p>

<p>
As expected, the most significant table in our <i>chat</i> database is a table
holding the users of the application; we name this table as the <i>user</i> table.
Users can be related to each other, that is they can mark selected users as <i>friends</i>,
or <i>block</i> other users if they feel so.
User relationships are kept, of course, in another table which we name as the <i>relation</i> table.
Users may send offline messages to each other; sent messages form the <i>message</i> table.
For now we don't need any other tables to setup our <i>chat</i> database.
</p>

<?php Globals::toc_item("schema", "The schema", 3); ?>

<p>
We quote the <i>chat</i> database schema in SQL/DDL format:
</p>

<?php Globals::file_quote("../../database/chat/schema.sql"); ?>

<?php Globals::toc_item("create", "Creating the database", 3); ?>

<p>
To create the database locally, you must download the file and run the following command
as the <i>root</i> database user:
</p>

<p>
<pre>
mysql -u root -p --force &lt;schema.sql
</pre>
</p>

<p>
However, there is no need to download the above file because this file
is included in the <code>spawk</code> tarball downloaded earlier.
The file <code>schema.sql</code> is located in the <code>chat</code> directory under the
<code>database</code> directory.
</p>

<p class="warning warningRed">
<strong>Warning!!!</strong>
<br>
If you have an existing <i>chat</i> database in your system, it will be deleted!
<br>
Database users <i>chat</i> and <i>chatadmin</i> will be dropped too!
</p>

<?php Globals::toc_item("populate", "Populating the database", 3); ?>

<p>
After creating the <i>chat</i> database and the <i>chat</i> and <i>chatadmin</i>
database users, the application is ready to accept new user registrations.
After registering at the application,
users will start chatting, developing relationships and sending messages to each other
and after a while we will have enough data in the <i>chat</i> database for
testing and demonstrating SPAWK.
However, we cannot wait for users to register in the <i>chat</i> application,
moreover we have not setup any web server, neither we have developed any server
or client side programs yet for the <i>chat</i> application to be up and running,
so we decide to populate the database manually using existing data files.
</p>

<p>
In order to populate the <i>chat</i> database with test data, visit once again
the <code>spawk</code> directory extracted from the downloaded SPAWK tarball.
There exists a <code>database</code> directory, and a <code>chat</code> subdirectory under
the <code>database</code> directory.
In this directory there exist two data files namely the <code>user.data</code> containing
2000 test user data, and the <code>relation.data</code> containing 30000 relationship pairs
between users.
Our intention is to load these data files into the database by using SPAWK
for the first time!
In the same directory there exists an AWK script named <code>populate.awk</code>.
Let's take a look:
</p>

<?php Globals::file_quote("../../database/chat/populate.awk", FALSE); ?>

<p>
The test data files are the <code>user.data</code> file containing 2000 users and the
<code>relation.data</code> file containing 30000 relationships.
It's obvious that the relationships must be loaded after the users, because of
the foreign key constraints between the corresponding tables.
<code>user.data</code> file has 7 tab separated fields, while
<code>relation</code> file has 3 tab separated fields:
<code>spawk_escape</code> function takes a string argument and enclose this
string in single quotes, after escaping single quotes found in the string,
e.g. <code>spawk_escape("panos")</code> returns the string <code>'panos'</code>,
while <code>spawk_escape("smith's")</code> returns the string <code>'smith\'s'</code>.
</p>

<?php Globals::toc_item("dataFiles", "The data files", 4); ?>

<p>
File <code>user.data</code>:
</p>

<p>
<pre class="output">
<?php system("head ../../database/chat/user.data"); ?>
...
</pre>
</p>

<p>
File <code>relation.data</code>:
</p>

<p>
<pre class="output">
<?php system("head ../../database/chat/relation.data"); ?>
...
</pre>
</p>

<p>
Before using SPAWK to load the data in the database, it's good to trace the
<code>populate.awk</code> script one section at a time:
</p>

<?php Globals::toc_item("populateSetup", "SPAWK setup", 4); ?>

<p>
<pre>
@load "spawk"

BEGIN {
	FS = "\t"
	spawk_sesami["dbuser"] = "chat"
	spawk_sesami["dbpassword"] = "xxx"
	spawk_sesami["dbname"] = "chat"
}
...
</pre>
</p>

<p>
The very first line of the script is just the inclusion of the <code>spawk</code>
dynamic extension. The extension has already been installed, possibly at the
</code>/usr/local/lib/gawk</code> directory. This directory is searched by default
from AWK, as <code>AWKLIBPATH</code> default value is "<code>/usr/local/lib/gawk</code>"
as stated in the relative
<a href="https://www.gnu.org/software/gawk/manual/html_node/AWKLIBPATH-Variable.html#AWKLIBPATH-Variable"
	target="_blank">section</a> of the AWK's user guide.
Just after loading the <code>spawk</code> extension, the <code>BEGIN</code> section
follows. Our test data files contain lines of tab separated fields, so we set the input
fields separator to the tab character.
Afterwards follows the <code>spawk_sesami</code> array setup, where the database access
credentials are set in order to use them later, whenever a database access is needed
to start a new client.
</p>

<?php Globals::toc_item("populateFiltering", "Filtering <i>user</i> data", 4); ?>

<p>
<pre>
...
NF == 7 {
	nf = 1
	login = $(nf++)
	registration = $(nf++)
	name = $(nf++)
	email = $(nf++)
	gender = $(nf++)
	byear = $(nf++)
	password = $(nf++)

	email = email ? spawk_escape(email) : "NULL"
	gender = gender ? spawk_escape(gender) : "NULL"
	byear = byear ? byear : "NULL"
...
</pre>
</p>

<p>
This part of the script uses AWK pattern <code>NF == 7</code> to filter input lines
containing just 7 fields, namely the login name, the registration timestamp,
the full name, the email address, the gender, the birth year
and the password (in SHA1 form), from the <code>user.data</code> file.
The email, gender and byear fields are allowed to be of null value,
so we set these values to the string <code>NULL</code> whenever these
fields are empty.
</p>

<?php Globals::toc_item("populateSubmit", "Submitting queries", 4); ?>

<p>
<pre>
...
if (spawk_submit("INSERT INTO `user` (" \
	"`login`, " \
	"`registration`, " \
	"`name`, " \
	"`email`, " \
	"`gender`, " \
	"`byear`, " \
	"`password`" \
") VALUES (" \
	spawk_escape(login) ", " \
	"FROM_UNIXTIME(" registration "), " \
	spawk_escape(name) ", " \
	email ", " \
	gender ", " \
	byear ", " \
	spawk_escape(password) \
")"))
next

if (!(++ucount % 1000))
print ucount, "users inserted"

next
...
</pre>
</p>

<p>
The above script section is the first SQL script to be submitted from AWK
via the SPAWK API. The function <code>spawk_submit</code> takes a string argument,
that is the query to be submitted to the MySQL/MariaDB database server.
The specific SQL script is a DML script;
DML stands for Data Manipulation Language (INSERT, UPDATE, DELETE etc)
in contradistinction with DQL which stands for Data Query Language
(SELECT, SHOW etc).
</p>

<p>
DML queries don't produce any result rows, while DQL queries produce result rows.
When submitting a query via <code>spawk_submit</code> function, a value is returned
to the caller; a value of 0 means that the query was processed successfully,
while non-zero values signal query failure.
This way one can check the returned value in order to proceed with further actions
or not.
In our case we skip to the next input line on failure, othewise we increment
a counter (<code>ucount</code>) and print a porogress message for every 1000 users
successfully inserted.
Of course, the fact that some of the user lines may fail to be inserted into the database, 
will cause problems later during the insertion of the relationships involving
the rejected users.
</p>

<p>
<pre>
...
NF == 3 {
	if (spawk_submit("INSERT INTO `relation` (" \
		"`user`, " \
		"`related`, " \
		"`relationship`" \
	") VALUES (" \
		spawk_escape($1) ", " \
		spawk_escape($2) ", " \
		spawk_escape($3) \
	")"))
	next

	if (!(++rcount % 1000))
	print rcount, "relations inserted"

	next
}
...
</pre>
</p>

<p>
The above section refers to lines with 3 fields, that is lines of
the <code>relation.data</code> file which contains 30000 user relationships.
For such lines an insertion SQL query is submitted, just as we did with the
user lines, and if the insertion fails we skip to the next input line,
else another counter is incremented and a progress message is printed every 1000
relationship rows inserted successfully.
</p>

<p>
<pre>
{
	print $0 ": syntax error" &gt;"dev/stderr"
}
</pre>
</p>

<p>
The last part of the script has nothing to do with SPAWK but it's good to catch up
input lines that do not conform to the given patterns of 7 and 3 fields per line.
Now it's time to actually run the script and populate the database:
</p>

<p>
<pre>
awk -f populate.awk user.data relation.data
<div class="output">1000 users inserted
2000 users inserted
1000 relations inserted
2000 relations inserted
3000 relations inserted
...
29000 relations inserted
30000 relations inserted</div>
</pre>
</p>

<p>
That's it! Our <i>chat</i> database has just been populated with 2000 users and
30000 relationships, so let the games begin!
</p>


<?php Globals::toc_item("counting", "Counting users, relationships etc", 2); ?>

<p>
Now that our <i>chat</i> database is populated with enough users and rleationships,
we can run SPAWK scripts in order to obtain useful information.
Most of this information can be extracted using plain vanilla SQL scripts, or other methods
such as PHP MySQL API, Node.js etc, but SPAWK may save you a lot of time if you know
how to use AWK effectively.
</p>

<?php Globals::toc_item("countingGender", "Counting genders", 3); ?>

<p>
For start let's count male and female users using plain SQL:
</p>

<?php Globals::file_quote("script/gender.sql"); ?>

<p>
Use the standard MySQL/MariaDB database client <code>mysql</code> to run the above script:
</p>

<p>
<pre>
mysql --user="chat" --password="xxx" chat &lt;gender.sql
</pre>
</p>

<p>
and produce the following results:
</p>

<p>
<pre class="output">
gender	COUNT(*)
NULL	153
MALE	925
FEMALE	922
</pre>
</p>

<p>
That is 925 male users, 922 female users and 153 users that have not specified their gender.
Now let's submit the very same query via SPAWK:
</p>

<?php Globals::file_quote("script/gender.awk"); ?>

<p>
Running the above script with SPAWK:
</p>

<p>
<pre>
awk -f gender.awk
</pre>
</p>

<p>
produces the following results:
</p>

<p>
<pre class="output">
	153
MALE	925
FEMALE	922
</pre>
</p>

<?php Globals::toc_item("countingRelgender", "Counting relationships by gender", 3); ?>

<p>
As you might have been thinking, it's not of much use to run SPAWK instead of
plain SQL just to count users by gender.
The results are the same and we used SPAWK just to submit a query and display the result rows.
But what if we make our query a little harder for plain SQL to carry out?
Assume for example that we want to count relationships grouped by users' gender.
Unless you are some kind of SQL wizard, you may find it difficult to produce
that kind of report using plain SQL.
So let's try SPAWK instead:
</p>

<?php Globals::file_quote("script/relgender.awk"); ?>

<p>
Run the above script with SPAWK:
</p>

<p>
<pre>
awk -f relgender.awk
</pre>
</p>

<p>
and you'll get the following results:
</p>

<p>
<pre class="output">
	2327
MALE	13936
FEMALE	13737
</pre>
</p>

<?php Globals::toc_item("countingFbgender", "Counting friends/blocked relationships by gender", 3); ?>

<p>
Let's make it even harder by adding the relatioship kind to the previous report.
We don't want just how many relationships exist for the two genders, but we
also want the kind of those relations.
</p>

<?php Globals::file_quote("script/fbgender.awk"); ?>

<p>
After running the above script with SPAWK we get the following results:
</p>

<p>
<pre class="output">
	BLOCKED	893
	FRIEND	1434
MALE	FRIEND	8318
MALE	BLOCKED	5618
FEMALE	FRIEND	8177
FEMALE	BLOCKED	5560
</pre>
</p>

<?php Globals::toc_item("popularToxic", "Popular and toxic users", 2); ?>

<p>
We are now going to write a SPAWK script that scans user login names and prints
<i>popular</i> users to the standard output sorted by <i>popularity</i>.
Users considered popular if they have been marked more times as <i>friend</i>
than as <i>blocked</i> by other users.
Popularity is the count of <i>friend</i> type relationships for a given user.
</p>

<?php Globals::file_quote("script/popular.awk"); ?>

<p>
After running the above script with SPAWK we get the following results:
</p>

<p>
<pre class="output">
acasley1u	3
glilleee	3
alinnitthk	4
cfarhertyb	4
...
jmcgraead	18
bbetje58	19
glaurentmq	19
rgagenfe	19
rvagg28		19
twickesb2	19
pcamamilena	23
</pre>
</p>

<p>
To print toxic users instead of popular ones, just change the last lines to:
</p>

<p>
<pre>
...
if (count["FRIEND"] &lt; count["BLOCKED"])
print login, count["BLOCKED"] | "sort -k2n"
...
</pre>
</p>

<?php Globals::toc_item("poptoxFilter", "Filtering popular/toxic users", 3); ?>

<p>
Now it's time to write a filter for filtering login user names supplied
as the first input field, in order to print only popular or only toxic users.
By default our script will filter popular users read from the input stream;
to print the toxic users we must use some kind of a flag variable.
</p>

<?php Globals::file_quote("script/poptox.awk"); ?>

<p>
To filter <i>popular</i> users read from standard input, run:
</p>

<p>
<pre>
awk -f poptox.awk
</pre>
</p>

<p>
or
</p>

<p>
<pre>
awk -v poptox="popular" -f poptox.awk
</pre>
</p>

<p>
The program reads login names from the input, so you have to supply login names in order
to get results in the output.
We don't have any convenient list of login names to test our program, so let's create one:
</p>

<?php Globals::file_quote("script/urand.awk"); ?>

<p>
We use the registration date as an epoch timestamp to filter a random
set of user login names:
</p>

<p>
<pre>
awk -f urand.awk &gt;test1
</pre>
</p>

<p>
Running the above script gives a total of 69 user login names in the file
<code>test1</code>.
</p>

<?php Globals::toc_item("poptoxDance", "Let's dance!", 3); ?>

<p>
To filter out the popular users from the <code>test1</code> file:
</p>

<p>
<pre>
awk -f poptox.awk test1
</pre>
</p>

<p>
There exist 53 popular users in our list.
For the toxic users run:
</p>

<p>
<pre>
awk -v poptox="toxic" -f poptox.awk test1
</pre>
</p>

<p>
There are just 11 toxic users in the list.
Let's use <code>comm</code> to check the results:
</p>

<p>
<pre>
comm -12 &lt;(awk -f poptox.awk test1 | sort) &lt;(awk -v poptox="toxic" -f poptox.awk test1 | sort)
</pre>
</p>

<p>
We see no results, so there are exist no users filtered both as popular or as toxic
by the <code>poptox.awk</code> script and that's a good sign.
No let's extract the users of the <code>test1</code> list that didn't
filtered neither as popular nor as toxic users:
</p>

<p>
<pre>
comm -23 &lt;(sort test1) &lt;(sort &lt;(awk -f poptox.awk test1) &lt;(awk -v poptox="toxic" -f poptox.awk test1))
</pre>
</p>

<p>
And voilà! The five missing users are:
</p>

<p>
<pre class="output">
abisgrovenp
ajzhakovdp
gcoramk9
hjozsefd
ksummerill4x
</pre>
</p>

<?php Globals::toc_item("poptoxReverse", "Astern propulsion", 3); ?>

<p>
Now let's use SPAWK one more time to check the relationships of those low profile users that
seem to have equal number of friends and blockers:
</p>

<?php Globals::file_quote("script/astern.awk"); ?>

<p>
Running the above script for the five non popular neither toxic users found in our list,
we get the expected results of equal number of friend/blocked relationships
for each one of these users:
</p>

<p>
<pre class="output">
abisgrovenp	9	9
ajzhakovdp	5	5
gcoramk9	10	10
hjozsefd	8	8
ksummerill4x	10	10
</pre>
</p>

<?php Globals::toc_item("recreatedb", "Recreating the <i>chat</i> database with SPAWK", 2); ?>

<p>
Now it's time to use SPAWK for submitting DDL queries.
For this we recreate the <i>chat</i> database using SPAWK.
As you might have already noticed, the <code>schema.sql</code> file follows a simple rule:
the query terminator character "<code>;</code>" is always placed at a line by itself.
Following this simple rule we can write a SPAWK script to create the <i>chat</i>
database and the relevant database users:
</p>

<?php Globals::file_quote("../../database/chat/create.awk"); ?>

<p>
Run the above query using SPAWK:
</p>

<p>
<pre>
awk -f create.awk schema.sql
</pre>
</p>

and populate the database:

<p>
<pre>
awk -f populate.awk user.data relation.data
</pre>
</p>

</div>
<!-- end of "main" division -->

<?php Globals::bottom(date("M d, Y", filemtime(__FILE__))); ?>

</body>

</html>
