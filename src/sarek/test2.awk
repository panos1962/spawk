@include "../../awklib/spawk.awk"

BEGIN {
	spawk_sarek = "./sarek.exe"
	spawk_sesami("chat", "xxx", "chat")

	if (reldel)
	self_delete()

	user_scan1()

	user_scan2()
	user_scan2(0)
	user_scan2(1)

	spawk_reset()
	spawk_sesami("chatdql", spawk_empty_password, "chat")

	user_scan2(2)

	spawk_reset()
	spawk_sesami("chat", "xxx", "chat")

	user_scan2(3)

	spawk_reset()
	spawk_sesami("chatadmin", "xxx", "chat")

	for (i in spawk_fis) {
		print "SCHEME " i

		spawk_submit("DESCRIBE user", i)
		while (spawk_fetchrow(x)) {
			for (i in x)
			print "[" i "] >>>" x[i] "<<<"
		}
	}
}

function self_delete() {
	spawk_submit("BEGIN WORK")
	spawk_submit("DELETE FROM `relation` WHERE `user` = `related`")
	print "Deleted self relations:", spawk_affected

	if (reldel > 1)
	return spawk_submit("COMMIT WORK")

	spawk_submit("ROLLBACK WORK")
	print "rollback"
}

function user_scan1(			user) {
	spawk_submit("SELECT `login` FROM `user` ORDER BY `login`")

	while (spawk_fetchrow(user))
	process_user(user)
}

function process_user(user,		relation) {
	spawk_submit("SELECT `related`, `relationship` FROM `relation` WHERE `user` = " \
		spawk_escape(user[1]) " ORDER BY `user`, `related`")

	while (spawk_fetchrow(relation)) {
		if (relation[1] == user[1])
		print user[1], relation[0]
	}
}

function user_scan2(fis,			user, i) {
	print "RESULT INDEXING SCHEME: >>" fis "<<"

	spawk_submit("SELECT * FROM `user` WHERE `login` LIKE 'zpotiera4'", fis)

	while (spawk_fetchrow(user, fis)) {
		for (i in user) 
		print "[" i "] >>" user[i] "<<"

		relation_scan2()
		relation_scan2(0)
		relation_scan2(1)
		relation_scan2(2)
		relation_scan2(3)
	}
}

function relation_scan2(fis,			relation, i) {
	print "\tRESULT INDEXING SCHEME: >>" fis "<<"

	spawk_submit("SELECT * FROM `relation` " \
		"WHERE `user` LIKE 'zpotiera4' " \
		"AND `related` LIKE 'h%'", fis)

	while (spawk_fetchrow(relation, fis)) {
		for (i in relation) 
		print "\t[" i "] >>" relation[i] "<<"
	}
}
