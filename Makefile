CC=gcc
CFLAGS=-g -Wall -Werror -pedantic
LFLAGS=-lm
BIN=pacman

all: build

build: player.o
	$(CC) $(CFLAGS) -o $(BIN) $< pacman.o $(LFLAGS)

player.o: player.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(BIN) player.o
