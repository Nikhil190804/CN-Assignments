set terminal png
set output 'q1_cwnd.png'
set title 'cwnd vs time'
set ylabel 'Congestion Window (Bytes)'
set xlabel 'Time (Seconds)'
plot 'q1.cwnd' using 1:2 with lines linewidth 2 linecolor 'blue' title 'cwnd'
quit