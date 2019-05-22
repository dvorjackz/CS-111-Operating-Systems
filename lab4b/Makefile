# NAME = Jack Zhang
# EMAIL = dvorjackz@ucla.edu
# ID = 004993345

CC = gcc
CFLAGS = -g -lmraa -lm

.SILENT:

default:
	$(CC) $(CFLAGS) -Wall -Wextra -o lab4b lab4b.c

clean:
	rm -f *.o *.tar.gz lab4b_list

dist:
	tar -czvf lab4b-004993345.tar.gz lab4b.c Makefile README

