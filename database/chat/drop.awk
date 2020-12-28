@load "spawk"

BEGIN {
	spawk_sesami["dbuser"] = "root"
	spawk_sesami["dbpassword"] = spawk_getpass()

	spawk_submit("DROP DATABASE chat")
	spawk_submit("DROP USER 'chat'@'localhost', 'chatdql'@'localhost', 'chatadmin'@'localhost'")
}
