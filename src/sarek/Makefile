EXEC = ./sarek.exe
OBJS = global.o main.o setup.o sarek.o message.o response.o result.o error.o
CFLAGS = `mysql_config --cflags`
LDFLAGS = `mysql_config --libs`

$(EXEC): $(OBJS)
	cc -o $(EXEC) $(OBJS) $(LDFLAGS)

strip: $(EXEC)
	strip $(EXEC)

test: $(EXEC)
	$(EXEC) -D -u chat -p xxx -d chat -c utf8 0

test2: $(EXEC)
	awk -f test2.awk

cleanup:
	@rm -f $(EXEC) $(OBJS)
