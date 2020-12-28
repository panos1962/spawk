<?php
require "../lib/standard.php";
?>

<!DOCTYPE html>

<html>

<head>
<?php Globals::head(); ?>
<style>
#main {
	max-width: 800px;
}
</style>
</head>

<body>

<?php Globals::top("about"); ?>

<!-- "main" division starts here -->
<div id="main">

<h1>About SPAWK</h1>

<p>
SPAWK stands for <b>S</b>QL <b>P</b>owered <b>AWK</b>,
that is AWK with database access ability.
Using SPAWK allows AWK to define, manipulate or query MySQL/MariaDB databases
via SQL query submission and data retrieval requests.
SPAWK was first implemented at 2004 by Panos Papadopoulos in a Dell Poweredge 2850
machine, running RedHat 5.0 RHEL and UNIFY DataServer 9.0 RDBMS.
Since then, SPAWK has been ported to MySQL/MariaDB databases and made available
to the public under the GPL3 licence.
</p>

</div>
<!-- end of "main" division -->

<?php Globals::bottom(date("M d, Y", filemtime(__FILE__))); ?>

</body>

</html>
