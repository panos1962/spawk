@load "spawk"

BEGIN {
	OFS = "\t"

	spawk_sesami["dbuser"] = "chat"
	spawk_sesami["dbpassword"] = "xxx"
	spawk_sesami["dbname"] = "chat"

	# Flag "poptox" is either "popular" or "toxic" and
	# can be specified in the command line. In case not
	# specified, then "poptox" is considered "popular".

	if (!poptox)
	poptox = "popular"
}

{
	delete count

	spawk_submit("SELECT `relationship` FROM `relation` " \
		"WHERE `related` = " spawk_escape($1))

	while (spawk_fetchrow(relation))
	count[relation[1]]++

	if (poptox == "popular")
	dif = count["FRIEND"] - count["BLOCKED"]

	else
	dif = count["BLOCKED"] - count["FRIEND"]

	if (dif > 0)
	print
}
