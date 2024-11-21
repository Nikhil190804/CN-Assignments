set terminal png
set output 'q3_cwnd.png'
set title 'cwnd vs time'
set ylabel 'Congestion Window (Bytes)'
set xlabel 'Time (Seconds)'
plot 'q3.cwnd' using 1:2 with lines linewidth 2 linecolor 'blue' title 'cwnd'
quit