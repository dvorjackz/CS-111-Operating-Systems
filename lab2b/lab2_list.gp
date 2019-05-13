#! /usr/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2b_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#
# output:
#	lab2b_1.png ... throughput (num of total operations for all threads over time)
#	lab2b_2.png ... threads and iterations that run (un-protected) w/o failure
#	lab2b_3.png ... threads and iterations that run (protected) w/o failure
#	lab2b_4.png ... cost per operation vs number of threads
#
# Note:
#	Managing data is simplified by keeping all of the results in a single
#	file.  But this means that the individual graphing commands have to
#	grep to select only the data they want.
#
#	Early in your implementation, you will not have data for all of the
#	tests, and the later sections may generate errors for missing data.
#

# general plot parameters
set terminal png
set datafile separator ","

# unset the kinky x axis
set xtics

set title "Throughput of Syncrhonization Methods"
set xlabel "Threads"
set logscale x 2
set xrange [1:24]
set ylabel "Throughput (operations/s)"
set logscale y 10
set output 'lab2b_1.png'
set key right top

plot \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/(($6)/($5))) title 'list w/mutex' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/(($6)/($5))) title 'list w/spin-lock' with linespoints lc rgb 'green'

set title "Wait-for-lock and avg operation time of mutex"
set ylabel "Time (s)"
set xlabel "Threads"
set output 'lab2b_2.png'

plot \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($8) title 'avg wait-for-lock time' with linespoints lc rgb 'black', \
     "< grep -e 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($7) title 'avg time per operation' with linespoints lc rgb 'yellow'

set title "Successful runs vs number of threads"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Successful runs"
set logscale y 10
set output 'lab2b_3.png'
set key right top

plot \
	"< grep 'list-id-none' lab2b_list.csv" using ($2):($3) title 'no lock w/ yields' with points lc rgb 'red', \
	"< grep list-id-m lab2b_list.csv" using ($2):($3) title 'mutex w/ yields' with points lc rgb 'blue', \
	"< grep list-id-s lab2b_list.csv" using ($2):($3) title 'spin w/ yields' with points lc rgb 'green'

set title "Throughput for mutex lock"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput (operations/s)"
set logscale y 10
set output 'lab2b_4.png'
set key right top

plot \
        "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/(($6)/($5))) title '1 list' with linespoints lc rgb 'black', \
	"< grep 'list-none-m,[0-9]*,1000,4' lab2b_list.csv" using ($2):(1000000000/(($6)/($5))) title '4 lists' with linespoints lc rgb 'green', \
	"< grep 'list-none-m,[0-9]*,1000,8' lab2b_list.csv" using ($2):(1000000000/(($6)/($5))) title '8 lists' with linespoints lc rgb 'red', \
	"< grep 'list-none-m,[0-9]*,1000,16' lab2b_list.csv" using ($2):(1000000000/(($6)/($5))) title '16 lists' with linespoints lc rgb 'blue'

set title "Throughput for spin lock"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput (operations/s)"
set logscale y 10
set output 'lab2b_5.png'
set key right top

plot \
        "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/(($6)/($5))) title '1 list' with linespoints lc rgb 'black', \
	"< grep 'list-none-s,[0-9]*,1000,4' lab2b_list.csv" using ($2):(1000000000/(($6)/($5))) title '4 lists' with linespoints lc rgb 'green', \
	"< grep 'list-none-s,[0-9]*,1000,8' lab2b_list.csv" using ($2):(1000000000/(($6)/($5))) title '8 lists' with linespoints lc rgb 'red', \
	"< grep 'list-none-s,[0-9]*,1000,16' lab2b_list.csv" using ($2):(1000000000/(($6)/($5))) title '16 lists' with linespoints lc rgb 'blue'