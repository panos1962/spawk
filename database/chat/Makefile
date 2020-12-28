SCHEMA = schema.sql
DATA = user.data relation.data
DROP = drop.awk

create: $(SCHEMA)
	mysql -u root -p --force <$(SCHEMA)

populate: $(DATA)
	awk -f populate.awk $(DATA)

drop: $(DROP)
	awk -f $(DROP)
