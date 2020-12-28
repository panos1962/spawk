SPARC = sparc
EXEC = ./$(SPARC).exe
OBJS = global.o main.o x.tab.o lex.yy.o error.o
LIBS = -ll

$(PROGRAM): $(EXEC)

$(EXEC): $(OBJS)
	cc -o $(EXEC) $(OBJS) $(LIBS)

$(SPARC).tab.c: $(SPARC).y
	bison -d $(SPARC).y

x.tab.c: $(SPARC).tab.c
	@-cmp -s x.tab.c $(SPARC).tab.c || cp $(SPARC).tab.c x.tab.c

lex.yy.o: lex.yy.c $(SPARC).tab.h

lex.yy.c: $(SPARC).l
	flex $(SPARC).l

x.tab.h: $(SPARC).tab.h
	@-cmp -s x.tab.h $(SPARC).tab.h || cp $(SPARC).tab.h x.tab.h

$(SPARC).tab.h: $(SPARC).y

cleanup:
	@rm -f $(EXEC) $(OBJS) $(SPARC).tab.[ch] x.tab.[ch] lex.yy.c

test: $(EXEC)
	$(EXEC)
