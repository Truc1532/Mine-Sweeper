CC=gcc
CFLAGS=-c -Wall
LDFLAGS=-lSDL2 -lSDL2_mixer

all: main

main: main.o
	$(CC) $(LDFLAGS) main.o -o a.out

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -f main.o
