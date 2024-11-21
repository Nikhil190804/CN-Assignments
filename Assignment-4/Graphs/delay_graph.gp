set term postscript eps color
set datafile separator ","
set output 'queue_delay.eps'
set ylabel 'delay'
set xlabel 'time'
plot 'queue_delay.csv' using 1:2