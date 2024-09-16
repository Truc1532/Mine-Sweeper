CC=gcc
CFLAGS=-Wall
LDFLAGS=-lSDL2 -lSDL2_mixer

all: main

main: 
	$(CC) $(CFLAGS) main.c $(LDFLAGS) -o minesweeper
clean:
	rm -f minesweeper
