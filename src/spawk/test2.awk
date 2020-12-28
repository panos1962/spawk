@load "./spawk"

BEGIN {
	OFS = "\t"

	spawk_verbose = 1

	spawk_sesami["dbhost"] = "localhost"
	spawk_sesami["dbuser"] = "chat"
	spawk_sesami["dbpassword"] = "xxx"
	spawk_sesami["dbname"] = "chat"
	spawk_sesami["dbcharset"] = "utf8"

	print "\nfis = undefined, fetchrow = undefined"
	if (spawk_submit("SELECT * FROM `user`") == 3) {
		if (spawk_fetchone(user))
		print_user(user)
	}

	print "\nfis = undefined, fetchrow = XXX"
	if (spawk_submit("SELECT * FROM `user`") == 3) {
		if (spawk_fetchone(user, "XXX"))
		print_user(user)
	}

	print "\nfis = 0, fetchrow = undefined"
	if (spawk_submit("SELECT * FROM `user`", 0) == 3) {
		if (spawk_fetchone(user))
		print_user(user)
	}

	print "\nfis = 0, fetchrow = XXX"
	if (spawk_submit("SELECT * FROM `user`", 0) == 3) {
		if (spawk_fetchone(user, "XXX"))
		print_user(user)
	}

	print "\nfis = 1, fetchrow = undefined"
	if (spawk_submit("SELECT * FROM `user`", 1) == 3) {
		if (spawk_fetchone(user))
		print_user(user)
	}

	print "\nfis = 1, fetchrow = XXX"
	if (spawk_submit("SELECT * FROM `user`", 1) == 3) {
		if (spawk_fetchone(user, "XXX"))
		print_user(user)
	}

	print "\nfis = 2, fetchrow = undefined"
	if (spawk_submit("SELECT * FROM `user`", 2) == 3) {
		if (spawk_fetchone(user))
		print_user(user)
	}

	print "\nfis = 2, fetchrow = XXX"
	if (spawk_submit("SELECT * FROM `user`", 2) == 3) {
		if (spawk_fetchone(user, "XXX"))
		print_user(user)
	}

	print "\nfis = 2, fetchrow = email"
	if (spawk_submit("SELECT * FROM `user`", 2) == 3) {
		if (spawk_fetchone(user, "email"))
		print_user(user)
	}

	print "\nfis = 3, fetchrow = undefined"
	if (spawk_submit("SELECT * FROM `user`", 3) == 3) {
		if (spawk_fetchone(user))
		print_user(user)
	}

	print "\nfis = 3, fetchrow = XXX"
	if (spawk_submit("SELECT * FROM `user`", 3) == 3) {
		if (spawk_fetchone(user, "XXX"))
		print_user(user)
	}
}

function print_user(user,		i) {
	for (i in user)
	print i, user[i]
}
