@include "../../awklib/spawk.awk"

BEGIN {
	spawk_sarek = "./sarek.exe"
	spawk_sesami("chat", "xxx", "chat")
	user_scan()
}

function user_scan(pat,			query, user) {
	query = "SELECT `login`, `email` FROM `user` "

	if (pat)
	query = query "WHERE `login` LIKE " spawk_escape(pat) " "

	query = query "ORDER BY `login`"

	spawk_submit(query)

	while (spawk_fetchrow(user))
	process_user(user)
}

function process_user(user,		relation) {
	print user[0]

	spawk_submit("SELECT `related`, `relationship` FROM `relation` WHERE `user` = " \
		spawk_escape(user[1]) " ORDER BY `user`, `related`")

	while (spawk_fetchrow(relation))
	print "\t" relation[0]
}
