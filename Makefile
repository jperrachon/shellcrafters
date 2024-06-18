CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99 -ggdb
OBJS = minish.o builtin_commands.o linea2argv.o ejecutar.o

minish: $(OBJS)
	$(CC) $(CFLAGS) -o minish $(OBJS)

minish.o: minish.c minish.h
	$(CC) $(CFLAGS) -c minish.c

builtin_commands.o: builtin_commands.c minish.h
	$(CC) $(CFLAGS) -c builtin_commands.c

linea2argv.o: linea2argv.c minish.h
	$(CC) $(CFLAGS) -c linea2argv.c

ejecutar.o: ejecutar.c minish.h
	$(CC) $(CFLAGS) -c ejecutar.c

clean:
	 rm -f *.o minish
