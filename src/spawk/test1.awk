@load "./spawk"

BEGIN {
	OFS = "\t"

	spawk_verbose = 1

	spawk_sesami["dbhost"] = "localhost"
	spawk_sesami["dbuser"] = "chat"
	spawk_sesami["dbpassword"] = "xxx"
	spawk_sesami["dbname"] = "chat"
	spawk_sesami["dbcharset"] = "utf8"

	spawk_submit("UPDATE `user` SET `name` = " spawk_escape("Παν'αγι'ώτ\nης\taaa\tbbb"))

	for (aa = 0; aa < 1; aa++) {
		if (spawk_submit("SELECT * FROM `user`", 3) != 3)
		continue

		while (spawk_fetchrow(user))
		process_user(user)
	}
}

function process_user(user,		i, relation) {
	for (i in user)
	print i, user[i]

	if (spawk_submit("SELECT * FROM `relation` WHERE `user` = '" user["login"] "'") != 3)
	return

	while (spawk_fetchrow(relation, ""))
	print relation[""]
}
