# NAME = Jack Zhang
# EMAIL = dvorjackz@ucla.edu

CC = gcc
CFLAGS = -g -o

.SILENT:

default:
	$(CC) $(CFLAGS) lab0 -Wall -Wextra lab0.c

clean:
	rm -rf lab0 *.o *.txt *.tar.gz

check: clean default successfullRun

dist: default
	tar -czvf lab0-420.tar.gz lab0.c README Makefile backtrace.png breakpoint.png

successfullRun:
	echo "420" > in.txt
	./lab0 --input in.txt --output out.txt
	if [[ $$? -ne 0 ]]; then \
		echo "Incorrect exit code upon sucessfully getting through the program."; \
	fi

	diff in.txt out.txt
	if [[ $$? -ne 0 ]]; then \
		echo "in.txt does not match out.txt."; \
	fi

segfaultRun:
	./lab0 --segfault --catch
	if [[ $$? -ne 4 ]]; then \
		echo "Incorrect exit code received for catching and handling segfault - should be 4" \
	fi
