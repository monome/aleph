# unset xtics
# unset ytics

set style line 1 lt rgb "#ff0000" lw 1 ps 1 pt 1
set style line 2 lt rgb "#00ff00" lw 1 ps 1 pt 2

plot "floatDeltaOut.txt" using 1:2 ls 1 title "float dy" w linespoints, \
"fixDeltaOut.txt" using 1:2 ls 2 title "fix dy" w linespoints

pause -1