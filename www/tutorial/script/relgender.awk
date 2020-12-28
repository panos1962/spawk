@load "spawk"

BEGIN {
	OFS = "\t"

	spawk_sesami["dbuser"] = "chat"
	spawk_sesami["dbpassword"] = "xxx"
	spawk_sesami["dbname"] = "chat"

	spawk_submit("SELECT `login`, `gender` FROM `user`")

	while (spawk_fetchrow(user)) {
		spawk_submit("SELECT COUNT(*) FROM `relation` " \
			"WHERE `user` = " spawk_escape(user[1]))
		spawk_fetchone(relcnt)
		count[user[2]] += relcnt[1]
	}

	for (gender in count)
	print gender, count[gender]
}
