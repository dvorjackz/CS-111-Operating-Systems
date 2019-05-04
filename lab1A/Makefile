# NAME = Jack Zhang
# EMAIL = dvorjackz@ucla.edu
# ID = 004993345

CC = gcc
CFLAGS = -g -o

.SILENT:

default:
	$(CC) $(CFLAGS) lab1a -Wall -Wextra lab1a.c

clean:
	rm -rf lab1a *.o *.txt *.tar.gz

dist: default
	tar -czvf lab1a-004993345.tar.gz lab1a.c README Makefile
