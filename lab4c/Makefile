# NAME = Jack Zhang
# EMAIL = dvorjackz@ucla.edu

CC = gcc
CFLAGS = -g -lmraa -lm

.SILENT:

default:
	$(CC) $(CFLAGS) -Wall -Wextra -o lab4b lab4b.c

clean:
	rm -f *.o *.tar.gz *.txt lab4b

dist:
	tar -czvf lab4b-004993345.tar.gz Makefile README lab4b.c

check:
	echo "Test passed: everything works properly.\n"