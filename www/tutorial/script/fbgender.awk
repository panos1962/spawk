@load "spawk"

BEGIN {
	OFS = "\t"

	spawk_sesami["dbuser"] = "chat"
	spawk_sesami["dbpassword"] = "xxx"
	spawk_sesami["dbname"] = "chat"

	spawk_submit("SELECT `login`, `gender` FROM `user`")

	while (spawk_fetchrow(user)) {
		spawk_submit("SELECT `relationship`, COUNT(*) " \
			"FROM `relation` WHERE `user` = " \
			spawk_escape(user[1]) " GROUP BY `relationship`")
		while (spawk_fetchrow(fbcnt))
		count[user[2] OFS fbcnt[1]] += fbcnt[2]
	}

	for (fbgender in count)
	print fbgender, count[fbgender]
}
