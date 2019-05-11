# NAME = Jack Zhang
# EMAIL = dvorjackz@ucla.edu

CC = gcc
CFLAGS = -lz -g -o

.SILENT:

default:
	$(CC) $(CFLAGS) lab1b-client -Wall -Wextra lab1b-client.c
	$(CC) $(CFLAGS) lab1b-server -Wall -Wextra lab1b-server.c

clean:
	rm -rf lab1b-client lab1b-server *.o *.txt *.tar.gz

dist: default
	tar -czvf lab1b-420.tar.gz lab1b-server.c lab1b-client.c README Makefile
