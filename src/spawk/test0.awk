@load "./spawk"

BEGIN {
	print spawk_verbose

	spawk_sesami["dbuser"] = "chat"
	spawk_sesami["dbpassword"] = "xxx"
	spawk_sesami["dbname"] = "chat"
	spawk_sesami["dbcharset"] = "utf8"

	print spawk_escape("ΑΒΓ'ΔΕΖ'ΗΘΙ\\ΚΛΜ\\ΝΞΟ")
	print spawk_escape("ΑΒΓ'ΔΕΖ'ΗΘΙ\\ΚΛΜ\\ΝΞΟ", 0)

	print spawk_submit("DELETE FROM `user` WHERE `login` = " spawk_escape("test"))
	print spawk_affected
	print spawk_info

	print spawk_submit("INSERT INTO `user` (`login`, `name`, `password`) " \
		"VALUES (" spawk_escape("test") ", " spawk_escape("spawk test") \
		", " spawk_escape("xxx") ")")
	print spawk_affected
	print spawk_info

	spawk_reset()
	spawk_sesami["dbuser"] = "chatadmin"
	spawk_sesami["dbpassword"] = "xxx"
	spawk_sesami["dbname"] = "chat"

	print spawk_submit("TRUNCATE TABLE `message`")
	print spawk_affected
	print spawk_info

	print spawk_submit("ALTER TABLE `message` AUTO_INCREMENT = 1")
	print spawk_affected
	print spawk_info

	print spawk_submit("INSERT INTO `message` (`sender`, `recipient`, `text`) " \
		"VALUES (" spawk_escape("test") ", " spawk_escape("test") \
		", " spawk_escape("xxx") ")")
	print spawk_insertid, spawk_affected
	print spawk_info

	print "VERBOSE_CHECK BEGIN" >"/dev/stderr"
	print spawk_submit("SELECT")
	spawk_verbose = 0
	print spawk_submit("SELECT")
	print "VERBOSE_CHECK END" >"/dev/stderr"
}
