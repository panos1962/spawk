CFLAGS = -I/cht/include -O
LIB = /cht/lib/cht.a

$(LIB): \
	$(LIB)(device.o) \
	$(LIB)(arenthili.o) \
	$(LIB)(onoma.o) \
	$(LIB)(diekat.o) \
	$(LIB)(prterm.o) \
	$(LIB)(numck.o) \
	$(LIB)(afmck.o) \
	$(LIB)(combs.o) \
	$(LIB)(crepeat.o) \
	$(LIB)(curtime.o) \
	$(LIB)(dtol.o) \
	$(LIB)(range.o) \
	$(LIB)(error.o) \
	$(LIB)(expire.o) \
	$(LIB)(filter.o) \
	$(LIB)(sigexam.o) \
	$(LIB)(gauss.o) \
	$(LIB)(rand.o) \
	$(LIB)(global.o) \
	$(LIB)(progname.o) \
	$(LIB)(sfopen.o) \
	$(LIB)(sfreopen.o) \
	$(LIB)(fnew.o) \
	$(LIB)(mktemp.o) \
	$(LIB)(splitf.o) \
	$(LIB)(strcpy.o) \
	$(LIB)(strnsave.o) \
	$(LIB)(strsave.o) \
	$(LIB)(strunc.o) \
	$(LIB)(tpm.o) \
	$(LIB)(putenv.o) \
	$(LIB)(getulh.o) \
	$(LIB)(usage.o) \
	$(LIB)(lexposo.o) \
	$(LIB)(lexevro.o) \
	$(LIB)(spellevro.o) \
	$(LIB)(fltprint.o) \
	$(LIB)(round.o) \
	$(LIB)(cfill.o) \
	$(LIB)(triad.o)
	chmod 660 $(LIB)
	chgrp 1001 $(LIB)
	chown root $(LIB)
	@echo $(LIB) is now up-to-date

.c.a:
	$(CC) -c $(CFLAGS) $<
	$(AR) $(ARFLAGS) $@ $*.o
	rm $*.o

test:
	make $(LIB)
