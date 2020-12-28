# SPAWK - SQL Powered AWK #

![](image/misc/spock512.png)

## AWK talking SQL to MySQL/MariaDB databases ##
------------------------------------------------



	@include "spawk"
	
	BEGIN {
		spawk_sesami("chat", "xxx", "chat")
	}
	
	{
		spawk_submit("SELECT `login`, `name` FROM `user` WHERE `login` LIKE " spawk_escape($0))

		while (spawk_fetchrow(user))
		process_user(user)
	}
	
	function process_user(user,		relation) {
		print user[0]
	
		spawk_submit("SELECT `related`, `relationship` FROM `relation` WHERE `user` = " spawk_escape(user[1]))
	
		while (spawk_fetchrow(relation))
		print "\t" relation[0]
	}
