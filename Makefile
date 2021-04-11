CC=gcc
CFLAGS=-Wall -std=c11

all: project3

project3: main.o
	$(CC) $(CFLAGS) main.o -o project3

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

clean:
	rm -f project3 main.o
