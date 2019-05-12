# NAME = Jack Zhang
# EMAIL = dvorjackz@ucla.edu
# ID = 004993345

CC = gcc
CFLAGS = -g -lrt -pthread

.SILENT:

default:
	$(CC) $(CFLAGS) -lprofiler -Wall -Wextra SortedList.c -o lab2_list lab2_list.c

tests: default

	./lab2_list --threads=1 --iterations=1000 --sync=s > lab2_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2_list.csv

clean:
	rm -rf lab2_list *.o

dist: clean graphs
	tar -czvf lab2b-004993345.tar.gz lab2_list.c SortedList.h SortedList.c lab2_list.csv lab2_list.gp Makefile README

graphs: clean tests
	./lab2_list.gp

profile:
	# lab2_list.prof is a dump where all of the output of pprof goes and is later redirected to profile.out
	rm -f lab2_list.prof
	LD_PRELOAD=/usr/lib64/libprofiler.so CPUPROFILE=lab2_list.prof ./lab2_list --threads=12 --iterations=1000 --sync=s
	pprof --text ./lab2_list ./lab2_list.prof > profile.out
	pprof --list=run_thread ./lab2_list ./lab2_list.prof >> profile.out
	rm -f lab2_list.prof

