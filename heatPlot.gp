set terminal png
set output 'heatmap16.png'
unset key
set title "Total Kinetic Energy @ 16\260"
set style increment default
set style data lines
set xtics border in scale 0,0 mirror norotate  autojustify
set ytics border in scale 0,0 mirror norotate  autojustify
set ztics border in scale 0,0 nomirror norotate  autojustify
set zrange [ * : * ] noreverse writeback
## Last datafile plotted: "$map1"
plot 'TKE16.dat' matrix with image