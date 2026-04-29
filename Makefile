CC     = gcc
CFLAGS = -Wall -Wextra -g
OBJS   = main.o helpers.o parse.o redirection.o argv.o path.o builtins.o

.PHONY: all clean run

all: kreash

kreash: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

run: kreash
	./kreash

clean:
	rm -f $(OBJS) kreash
