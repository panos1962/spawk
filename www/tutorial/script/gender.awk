@load "spawk"

BEGIN {
	spawk_sesami["dbuser"] = "chat"
	spawk_sesami["dbpassword"] = "xxx"
	spawk_sesami["dbname"] = "chat"

	spawk_submit("SELECT `gender`, COUNT(*) FROM `user` GROUP BY `gender`", 0)

	while (spawk_fetchrow(count))
	print count[0]
}
