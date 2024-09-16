CC=gcc
LDFLAGS=-lSDL2 -lSDL2_mixer

all: main

main: main.c
	$(CC) main.c $(LDFLAGS) -o minesweeper
clean:
	rm -f minesweeper
