NAME = spawk
SOURCE = $(NAME).c
OBJECT = $(NAME).o
LIBLOC = $(NAME).so
LIBSYS = /usr/local/lib/gawk/$(NAME).so
CFLAGS = `mysql_config --cflags`
LDFLAGS = `mysql_config --libs`

$(LIBLOC): $(OBJECT)
	gcc -o $(LIBLOC) -shared $(OBJECT) $(LDFLAGS)

$(OBJECT): $(SOURCE)
	gcc -fPIC -shared -c -O -g $(CFLAGS) $(SOURCE)

test: $(LIBLOC)
	sh ./test.sh
	gawk -f test1.awk >test1.out; tail -n 20 test1.out

install: $(LIBSYS)

$(LIBSYS): $(LIBLOC)
	@sudo cp $(LIBLOC) $(LIBSYS)

installtest: $(LIBSYS)
	gawk -f test1.awk >test1.out
	tail test1.out

uninstall:
	@sudo rm -f $(LIBSYS)

cleanup:
	@rm -f $(OBJECT) $(LIBLOC) test[1-9].out
