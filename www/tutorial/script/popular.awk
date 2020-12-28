@load "spawk"

BEGIN {
	OFS = "\t"

	spawk_sesami["dbuser"] = "chat"
	spawk_sesami["dbpassword"] = "xxx"
	spawk_sesami["dbname"] = "chat"

	spawk_submit("SELECT `login` FROM `user`")

	while (spawk_fetchrow(user))
	process_user(user[1])
}

function process_user(login,		relation, count) {
	spawk_submit("SELECT `relationship` FROM `relation` " \
		"WHERE `related` = " spawk_escape(login))

	while (spawk_fetchrow(relation))
	count[relation[1]]++

	if (count["FRIEND"] > count["BLOCKED"])
	print login, count["FRIEND"] | "sort -k2n"
}
