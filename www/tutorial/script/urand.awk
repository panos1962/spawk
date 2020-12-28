@include "spawk.awk"

BEGIN {
	spawk_sesami("chat", "xxx", "chat")
	spawk_submit("SELECT `login`, UNIX_TIMESTAMP(`registration`) FROM `user`")
	while (spawk_fetchrow(user)) {
		if ((user[2] % 100) > 95)
		print user[1]
	}
}
