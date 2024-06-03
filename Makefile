CC = gcc
CFLAGS = -Wall
OBJS = minish.o

minish: $(OBJS)
	$(CC) $(CFLAGS) -o minish $(OBJS)

clean:
	rm -f minish $(OBJS)
