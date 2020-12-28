@include "../../awklib/spawk.awk"

BEGIN {
	spawk_sarek = "./sarek.exe"
	spawk_sesami("chat", "xxx", "chat")

	spawk_submit("DELETE FROM `message`")
	print spawk_affected, spawk_insertid
	print spawk_info

	spawk_submit("INSERT INTO `message` (`sender`, `recipient`, `text`) VALUES (" \
		"'abrimson', 'aagatev', " spawk_escape("Καλημέρα Γιώργο.\nΤι κάνεις;") ")")
	print spawk_affected, spawk_insertid
	print spawk_info

	spawk_submit("INSERT INTO `message` (`sender`, `recipient`, `text`) VALUES (" \
		"'abrimson', 'aagatev', " spawk_escape("abc\txxx\tzzz") ")")
	print spawk_affected, spawk_insertid
	print spawk_info

	spawk_submit("SELECT * FROM `message`")

	while (spawk_fetchrow(message))
	print message[0]
}
