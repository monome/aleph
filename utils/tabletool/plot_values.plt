# unset xtics
# unset ytics

set style line 1 lt rgb "#ff0000" lw 2 ps 1 pt 0
set style line 2 lt rgb "#00ff00" lw 2 ps 1 pt 0
set style line 3 lt rgb "#0000ff" lw 2 ps 1 pt 0
set style line 4 lt rgb "#cccc00" lw 2 ps 1 pt 0
set style line 5 lt rgb "#cc00cc" lw 2 ps 1 pt 0
set style line 6 lt rgb "#00cc00" lw 2 ps 1 pt 0
set style line 7 lt rgb "#ff8800" lw 2 ps 1 pt 0
set style line 8 lt rgb "#8800ff" lw 2 ps 1 pt 0
set style line 9 lt rgb "#00ff88" lw 2 ps 1 pt 0

plot "floatValueOut.txt" using 1:2 ls 1 title "float y" w linespoints, \
"fixValueOut.txt" using 1:2 ls 2 title "fix y" w linespoints

pause -1