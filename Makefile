TARBALL = www/download/spawk.tar.gz
TARFLIST = lib/spawk.flist

LIB = src/spawk/spawk.so
LIBDIR = /usr/local/lib/gawk
SPAWK = $(LIBDIR)/spawk.so

NOTARGET = "NEVER_EXISTS"

all:
	@echo "make [ spawk | check | database | cleanup | install (as root) | uninstall ]"

spawk: $(LIB)

check:
	@ls -l $(LIB) $(SPAWK)

database: $(NOTARGET)
	@(cd database/chat && make create && make populate)

cleanup:
	@(cd src/spawk && make -s cleanup)

$(LIB):
	@(cd src/spawk && make)

install: $(SPAWK)
	@ls -l $(SPAWK)

$(SPAWK): $(LIB)
	@[ -d $(LIBDIR) ] || mkdir -p $(LIBDIR)
	@sudo cp $(LIB) $(SPAWK)

uninstall:
	@sudo rm -f $(SPAWK)

tar:
	@rm -f $(TARBALL)
	@make -s $(TARBALL)

$(TARBALL): $(TARFLIST)
	@(cd .. && tar zcf spawk/$(TARBALL) --no-recursion --files-from spawk/$(TARFLIST)) && ls -l $(TARBALL)

$(NOTARGET):
