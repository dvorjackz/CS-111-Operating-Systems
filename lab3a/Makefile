# NAME: Jack Zhang,Edward Zhang
# EMAIL: dvorjackz@ucla.edu,edward123@gmail.com

.SILENT:

CC = gcc
CFLAGS = -Wall -Wextra -o lab3a
LDLIBS = -lm

build:
	$(CC) $(CFLAGS) $(LDLIBS) lab3a.c

clean:
	rm -f *.tar.gz *.o lab3a

dist:
	tar -czvf lab3a-004993345.tar.gz Makefile README ext2_fs.h lab3a.c
