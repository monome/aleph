#
set style line 1 lt rgb "#ff0000" lw 2 ps 1 pt 0
set style line 2 lt rgb "#0000ff" lw 2 ps 1 pt 0
set style line 3 lt rgb "#00ffff" lw 2 ps 1 pt 0
#
plot "mono_dbg.txt" using 1:2 ls 1 w lines, \
     "mono_dbg.txt" using 1:3 ls 2 w lines, \
     "mono_dbg.txt" using 1:4 ls 3 w lines
#
pause -1