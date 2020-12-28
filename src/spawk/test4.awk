@load "./spawk"

BEGIN {
	OFS = "\t"

	spawk_verbose = 1

	spawk_sesami["dbhost"] = "localhost"
	spawk_sesami["dbuser"] = "chat"
	spawk_sesami["dbpassword"] = "xxx"
	spawk_sesami["dbname"] = "chat"
	spawk_sesami["dbcharset"] = "utf8"

	tab = tab ? "@" : "\t"
	s = "Παναγιώτης" tab "Δώρα" tab "maria" tab "akis" tab "Ελευθερία" \
		"\naaa" tab "bbb"

	for (aa = 1; 1; aa++) {
		if (!(aa % 1000000))
		print aa >"/dev/stderr"

		print s
		print spawk_escape(s)
		print spawk_escape(s, 0)
	}
}
