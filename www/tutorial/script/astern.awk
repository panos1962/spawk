@load "spawk"

BEGIN {
	OFS = "\t"

	spawk_sesami["dbuser"] = "chat"
	spawk_sesami["dbpassword"] = "xxx"
	spawk_sesami["dbname"] = "chat"
}

{
	delete count

	spawk_submit("SELECT `relationship` FROM `relation` " \
		"WHERE `related` = " spawk_escape($1))

	while (spawk_fetchrow(relation))
	count[relation[1]]++

	print $1, count["FRIEND"], count["BLOCKED"]
}
