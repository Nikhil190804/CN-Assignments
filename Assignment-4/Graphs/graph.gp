set term postscript eps color
set output 'cwnd_old.eps'
set ylabel 'cwnd'
set xlabel 'time'
plot 'old.cwnd' using 1:2