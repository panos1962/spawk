<?php
require "../lib/standard.php";
?>

<!DOCTYPE html>

<html>

<head>
<?php Globals::head(); ?>
</head>

<body>

<?php Globals::top("download"); ?>

<!-- "main" division starts here -->
<div id="main">

<?php Globals::toc_item("top", "SPAWK Download &amp; Installation", 1); ?>

<p>
To use SPAWK you have to download SPAWK tarball and install SPAWK components
to your local Linux system.
You must know how to download files, how to extract files from <code>tar</code>
archives, how to compile and link programs and, finally, you must have administrative
permissions to install the SPAWK files.
That's not a big deal, but if you are not a programmer or you don't have administrative
permissions, you may find it difficult to do so.
</p>

<?php Globals::toc_item("fastTrack", "Fast track", 2); ?>

<p>
This is for the lazy. Good luck!
</p>

<p>
<pre>
# Check for gawk version 3.1 or later
gawk --version

# Check for MySQL/MariaDB version 5.7 or later
mysql_config --version

# Download
wget spawk.info/download/spawk.tar.gz
tar zxf spawk.tar.gz
cd spawk

# Install
sudo make uninstall
make cleanup
make spawk
sudo make install

# Create and populate "chat" database
make database

# Cleanup
cd ..
rm -rf spawk spawk.tar.gz
</pre>
</p>

<p>
If you are lucky, you must now have SPAWK installed in your local system.
Have fun!
</p>

<?php Globals::toc_item("prerequisites", "Prerequisites", 2); ?>

<p>
You must be able to compile and link MySQL/MariaDB client programs in your local system,
so the MySQL/MariaDB C API header files, the MySQL/MariaDB client libraries,
the C compiler, the linker and the <code>make</code> utility program must be installed
to your local system.
</p>

<ul>

<li>
<p>
gawk version 3.1 or later.
</p>
</li>

<li>
<p>
MySQL/MariaDB version 5.7 or later.
The <a href="https://dev.mysql.com/doc/refman/5.7/en/mysql-config.html"
target="mysqlconfig"><code>mysql_config</code></a> script and
the <a href="https://dev.mysql.com/doc/refman/5.7/en/c-api-implementations.html"
target="mysqllib"><code>libmysqlclient</code></a>
library must be installed in your system.
</p>
</li>

<li>
<p>
The <code>gcc</code> (C compiler, libraries, linker etc) must be installed in your system.
</p>
</li>

</ul>

<?php Globals::toc_item("download", "Download the SPAWK tarball", 2); ?>

<p>
You can download the SPAWK tarball either by clicking the following link tab:
</p>

<p>
[&nbsp;<a href="spawk.tar.gz" download="spawk.tar.gz">Download</a>&nbsp;]
</p>
</p>

<p>
or by hand using CLI at your terminal:
</p>

<p>
<pre>
wget spawk.info/download/spawk.tar.gz
</pre>
</p>

<?php Globals::toc_item("extract", "Extract files from SPAWK tarball", 2); ?>

<p>
After downloading the SPAWK tarball file to your local system, you have
to decompress, extract, compile and install the spawk files.
Either use you GUI to extract files by clicking the <i>Extract here</i> option,
or use CLI instead:
</p>

<p>
<pre>
tar zxf spawk.tar.gz
</pre>
</p>

<p>
After decompression and extraction of the SPAWK files a <code>spawk</code>
directory must have been created.
You can now remove the tarball from your local system, but it's safer to
keep the file until you can finally manage to have SPAWK up and running.
Now move yourself to the <code>spawk</code> directory:
</p>

<p>
<pre>
cd spawk
</pre>
</p>

<?php Globals::toc_item("compile", "Compile and link SPAWK extension library", 2); ?>

<p>
To compile and link the SPAWK extension library use CLI:
</p>

<p>
<pre>
make spawk
<div class="output">
make[1]: Entering directory…
gcc -fPIC -shared -c -O -g `mysql_config --cflags` spawk.c
gcc -o spawk.so -shared spawk.o `mysql_config --libs`
make[1]: Leaving directory…
make [ check | database | cleanup | install (as root) | uninstall ]
<div>
</pre>
</p>

<p>
Unless you have missing programs or libraries in you local system,
you must see something like the above output at your terminal's screen.
</p>

<?php Globals::toc_item("database", "Create and populate the <i>chat</i> database", 2); ?>

<p>
It's time to test SPAWK. For this you have to create and populate the <i>chat</i>
database. The database schema and enough data for testing SPAWK are provided in the SPAWK
tarball and have already been downloaded, so return to your terminal once again, ensure
that you are still located at the <code>spawk</code> directory and run the following command:
</p>

<p>
<pre>
make database
</pre>
</p>

<p>
After prompting for the database root password, an empty database will be created.
Then SPAWK will load 2000 <i>users</i> and 30000 <i>relatioships</i> into
the newly created database.
The <i>chat</i> database can be used for SPAWK testing and learning.
The whole <a href="../tutorial" target="tutorial">tutorial</a> is based on
the <i>chat</i> database.
Later on, you can use SPAWK at any time to drop the database and the related database users:
</p>

<?php Globals::file_quote("../../database/chat/drop.awk"); ?>

<?php Globals::toc_item("install", "Install SPAWK", 2); ?>

<p>
Provided that there were no errors encountered,
you are now ready to install the SPAWK files to your local system directories.
This must be done with administrative permissions, so you have to run either:
</p>

<p>
<pre>
sudo make install
</pre>
</p>

<p>
or
</p>

<p>
<pre>
su -c "make install"
</pre>
</p>

<p>
That's it!
You have now SPAWK installed in your local system.
Have fun!
</p>

<?php Globals::toc_item("uninstall", "Uninstall SPAWK", 2); ?>

<p>
If you don't need SPAWK anymore, you can uninstall the SPAWK files from your local system.
This must be done with administrative permissions, so you have to run either:
</p>

<p>
<pre>
sudo make uninstall
</pre>
</p>

<p>
or
</p>

<p>
<pre>
su -c "make uninstall"
</pre>
</p>

</div>
<!-- end of "main" division -->

<?php Globals::bottom(date("M d, Y", filemtime(__FILE__))); ?>

</body>

</html>
