set style line 1 lt 2 lc rgb "black" lw 2
set style line 2 lt 4 lc rgb "purple" lw 1
set style line 3 lt 4 lc rgb "orange" lw 1
set style line 4 lt 4 lc rgb "blue" lw 2
set style line 5 lt 4 lc rgb "grey" lw 1
set style line 6 lt 4 lc rgb "green" lw 1
set style line 7 lt 4 lc rgb "cyan" lw 1
set style line 8 lt 2 lc rgb "red" lw 2
set style line 9 lt 2 lc rgb "#a0a0f0" lw 0.5
set palette rgbformulae 8, 9, 7
set object 1 rect from screen 0, 0, 0 to screen 1, 1, 0 behind
#set object 1 rect fc  rgb "gray"  fillstyle solid 1.0  border -1
set style fill solid 0.30 border

set size ratio 1.0
unset border
unset key
set xlabel "X"
set ylabel "Y"
set size ratio -1

plot \
     "< test/tools/gnuplotitems.pl cleared results/troute.txt" using 1:2 with linespoints ls 5 title "cleared", \
     "< test/tools/gnuplotitems.pl cand results/troute.txt" using 1:2 with linespoints ls 4 title "candidate", \
     "< test/tools/gnuplotitems.pl shortcut results/troute.txt" using 1:2 with linespoints ls 8 title "shortcut", \
     "< test/tools/gnuplotitems.pl solution results/troute.txt" using 1:2 with linespoints ls 1 title "solution", \
     "results/footprint.txt" using 1:2 with linespoints ls 3 title "footprint"

pause -1

set pm3d explicit map
splot \
      "results/terrain.txt" using 1:2:3 with pm3d, \
     "< test/tools/gnuplotitems.pl cleared results/troute.txt" using 1:2:3 with linespoints ls 5 title "cleared", \
     "< test/tools/gnuplotitems.pl cand results/troute.txt" using 1:2:3 with linespoints ls 4 title "candidate", \
     "< test/tools/gnuplotitems.pl shortcut results/troute.txt" using 1:2:3 with points ls 2 title "shortcut", \
     "< test/tools/gnuplotitems.pl solution results/troute.txt" using 1:2:3 with linespoints ls 1 title "solution"

pause -1

# set pm3d at s depthorder hidden3d 9
set terminal wxt
set output
unset border
unset key
unset xtics
unset ytics
unset ztics
unset xlabel
unset ylabel
unset colorbox
set parametric
#unset surface
set hidden3d
set view 60,72,1,0.5

splot \
      "results/terrain.txt" using 1:2:3 with lines, \
     "< test/tools/gnuplotitems.pl shortcut results/troute.txt" using 1:2:3 with points ls 2 title "shortcut", \
     "< test/tools/gnuplotitems.pl cand results/troute.txt" using 1:2:3 with linespoints ls 4 title "candidate", \
     "< test/tools/gnuplotitems.pl solution results/troute.txt" using 1:2:($3+300) with linespoints ls 1 title "solution"
pause -1

plot \
     "< test/tools/gnuplotitems.pl solution results/troute.txt" using 0:3 with linespoints ls 1 title "solution"
pause -1

