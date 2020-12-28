@include "../../awklib/spawk.awk"

BEGIN {
	spawk_sarek = "./sarek.exe"
	spawk_sesami("chat", "xxx", "chat")

	concordant = 0
	contradictive = 0
	introvertive = 0
	narkissus = 0
	pathetic = 0

	user_scan()
}

function user_scan(			relation) {
	spawk_submit("SELECT `user`, `related`, `relationship` FROM `relation`")

	while (spawk_fetchrow(relation))
	process_user(relation)

	print "concordant:", concordant / 2
	print "contradictive:", contradictive / 2
	print "introverted:", introverted
	print "narkissus:", narkissus
	print "pathetic:", pathetic
}

function process_user(relation,		reverse) {
	if (relation[1] == relation[2]) {
		introverted++

		if (relation[3] == "FRIEND")
		narkissus++

		else
		pathetic++
		
		print "self relation:", relation[0] >"/dev/stderr"
		return
	}

	spawk_submit("SELECT `relationship` FROM `relation` " \
		"WHERE `user` = " spawk_escape(relation[2]) " " \
		"AND `related` = " spawk_escape(relation[1]))

	while (spawk_fetchrow(reverse)) {
		if (reverse[1] == relation[3])
		concordant++

		else
		contradictive++
	}
}
