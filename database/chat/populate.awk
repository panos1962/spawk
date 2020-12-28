@load "../../src/spawk/spawk"

BEGIN {
	FS = "\t"
	spawk_sesami["dbuser"] = "chat"
	spawk_sesami["dbpassword"] = "xxx"
	spawk_sesami["dbname"] = "chat"
}

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
	")") != 2)
	next

	if (!(++ucount % 1000))
	print ucount, "users inserted"

	next
}

NF == 3 {
	if (spawk_submit("INSERT INTO `relation` (" \
		"`user`, " \
		"`related`, " \
		"`relationship`" \
	") VALUES (" \
		spawk_escape($1) ", " \
		spawk_escape($2) ", " \
		spawk_escape($3) \
	")") != 2)
	next

	if (!(++rcount % 1000))
	print rcount, "relations inserted"

	next
}

{
	print $0 ": syntax error" >"/dev/stderr"
}
