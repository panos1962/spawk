@load "spawk"

# We intend to create a new database, so we have to use an administrative
# database account.

BEGIN {
	spawk_sesami["dbuser"] = "root"
	spawk_sesami["dbpassword"] = spawk_getpass("Enter root password: ")
}

# Skip blank lines.

/^[[:space:]]*$/ {
	next
}

# Skip comment lines.

/^[[:space:]]*--.*$/ {
	next
}

# Line containing just a ";" character are considered to be query
# terminators, so run the query constructed already.

/^[[:space:]]*;[[:space:]]*$/ {
	spawk_submit(query)

	# After submitting the query, reset the query to an empty string.

	query = ""
	next
}

# Add this line to the current query.

{
	query = query " " $0
}
