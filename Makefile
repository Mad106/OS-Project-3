CC=gcc
CFLAGS=-Wall -std=c11

all: project3

project3: part6_10.o
	$(CC) $(CFLAGS) part6_10.o -o project3

part6_10.o: part6_10.c
	$(CC) $(CFLAGS) -c part6_10.c -o part6_10.o

clean:
	rm -f project3 part6_10.o