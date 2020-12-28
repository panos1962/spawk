PROGRAM = sparc
EXEC = ./sparc.exe
OBJS = global.o main.o craps.tab.o lex.yy.o error.o text.o variable.o \
	table.o column.o sort.o generate.o object.o archive.o \
	report.o filter.o interpret.o usage.o progname.o cht.o
LIBS = -ll

$(EXEC): $(OBJS)
	cc -o $(EXEC) $(OBJS) $(LIBS)

sparc.tab.c: $(PROGRAM).y
	bison -d $(PROGRAM).y

craps.tab.c: sparc.tab.c
	@-cmp -s craps.tab.c sparc.tab.c || cp sparc.tab.c craps.tab.c

lex.yy.o: lex.yy.c sparc.tab.h

lex.yy.c: $(PROGRAM).l
	lex $(PROGRAM).l

craps.tab.h: sparc.tab.h
	@-cmp -s craps.tab.h sparc.tab.h || cp sparc.tab.h craps.tab.h

sparc.tab.h: $(PROGRAM).y

cleanup:
	@rm -f $(EXEC) $(OBJS) sparc.tab.c craps.tab.c lex.yy.c sparc.tab.h craps.tab.h

test: $(EXEC) test1.data test1.ro
	@make $(EXEC)
	$(EXEC) -c -b test1.rs
	awk -f ../../cht/awklib/rptlib.awk.src -f ../../cht/awklib/std.awk.src -f test1.ro test1.data

test1.ro: test1.rs
	$(EXEC) test1.rs >test1.ro
