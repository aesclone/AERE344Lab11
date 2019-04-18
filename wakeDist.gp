set terminal png
set output 'wakedist16.png'
set grid
set autoscale
unset key
set grid
set xrange [*:*] reverse
set title "Wake Distribution @ 16\260"
set xlabel "Coefficient of Pressure"
set ylabel "Position (m)"
plot 'WakeDist16.dat' using 1:2 with lines