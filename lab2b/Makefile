# NAME = Jack Zhang
# EMAIL = dvorjackz@ucla.edu

CC = gcc
CFLAGS = -g -lrt -pthread

.SILENT:

default:
	$(CC) $(CFLAGS) -lprofiler -Wall -Wextra SortedList.c -o lab2_list lab2_list.c

tests: default

	# lab2b_1.png
	./lab2_list --threads=1 --iterations=1000 --sync=s > lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=s >> lab2b_list.csv

	# lab2b_2.png
	./lab2_list --threads=1 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2b_list.csv

	# lab2b_3.png
	-./lab2_list --threads=1 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv

	./lab2_list --threads=1 --iterations=10 --sync=s --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=20 --sync=s --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=40 --sync=s --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=80 --sync=s --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=10 --sync=s --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=20 --sync=s --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=40 --sync=s --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=80 --sync=s --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=10 --sync=s --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=20 --sync=s --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=40 --sync=s --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=80 --sync=s --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=10 --sync=s --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=20 --sync=s --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=40 --sync=s --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=80 --sync=s --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=10 --sync=s --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=20 --sync=s --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=40 --sync=s --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=80 --sync=s --yield=id --lists=4 >> lab2b_list.csv

	./lab2_list --threads=1 --iterations=10 --sync=m --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=20 --sync=m --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=40 --sync=m --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=80 --sync=m --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=10 --sync=m --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=20 --sync=m --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=40 --sync=m --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=80 --sync=m --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=10 --sync=m --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=20 --sync=m --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=40 --sync=m --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=80 --sync=m --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=10 --sync=m --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=20 --sync=m --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=40 --sync=m --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=80 --sync=m --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=10 --sync=m --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=20 --sync=m --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=40 --sync=m --yield=id --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=80 --sync=m --yield=id --lists=4 >> lab2b_list.csv

	# lab2b_4.png
	# ./lab2_list --threads=1 --iterations=1000 --lists=1 --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=2 --iterations=1000 --lists=1 --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=4 --iterations=1000 --lists=1 --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=8 --iterations=1000 --lists=1 --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=12 --iterations=1000 --lists=1 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --lists=4 --sync=m >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --lists=4 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --lists=4 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --lists=4 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --lists=4 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --lists=8 --sync=m >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --lists=8 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --lists=8 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --lists=8 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --lists=8 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv

	# lab2b_5.png
	# ./lab2_list --threads=1 --iterations=1000 --lists=1 --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=2 --iterations=1000 --lists=1 --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=4 --iterations=1000 --lists=1 --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=8 --iterations=1000 --lists=1 --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=12 --iterations=1000 --lists=1 --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --lists=4 --sync=s >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --lists=4 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --lists=4 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --lists=4 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --lists=4 --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --lists=8 --sync=s >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --lists=8 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --lists=8 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --lists=8 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --lists=8 --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv

clean:
	rm -rf lab2_list *.o *tar.gz *~

dist: clean graphs
	tar -czvf lab2b-004993345.tar.gz lab2_list.c SortedList.h SortedList.c lab2b_list.csv lab2_list.gp Makefile README lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png profile.out

graphs: clean tests
	./lab2_list.gp

profile:
	# lab2_list.prof is a dump where all of the output of pprof goes and is later redirected to profile.out
	rm -f lab2_list.prof
	LD_PRELOAD=/usr/lib64/libprofiler.so CPUPROFILE=lab2_list.prof ./lab2_list --threads=12 --iterations=1000 --sync=s
	pprof --text ./lab2_list ./lab2_list.prof > profile.out
	pprof --list=run_thread ./lab2_list ./lab2_list.prof >> profile.out
	rm -f lab2_list.prof

