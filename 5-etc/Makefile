# Makefile

CC = gcc
CFLAGS = -Wall

.PHONY: all
all: server client

client: client.o np_common.o
	$(CC) -o $@ client.o np_common.o

server: server.o np_common.o
	$(CC) -o $@ server.o np_common.o

client.o: client.c
	$(CC) $(CFLAGS) -c client.c

server.o: server.c
	$(CC) $(CFLAGS) -c server.c

np_common.o: np_common.c
	$(CC) $(CFLAGS) -c np_common.c

np_common.o: np_common.h

.PHONY: clean
clean:
	rm -f server client *.o

