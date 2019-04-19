set terminal png
set output 'wakedist.png'
set grid
set autoscale
set title "Wake Distributions"
set xlabel "Coefficient of Pressure"
set ylabel "Position (m)"
plot 'WakeDist04.dat' u 1:2 w l t "4\260", 'WakeDist08.dat' u 1:2 w l t "8\260", 'WakeDist12.dat' u 1:2 w l t "12\260", 'WakeDist16.dat' u 1:2 w l t "16\260"