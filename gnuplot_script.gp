set terminal pngcairo size 360,492
set output 'graph/mergegraph.png'
set size 1, 1
set key font 'Arial, 20'
set xlabel 'Elapsed Time'font 'Arial, 20' 
set xtics 40 font 'Arial, 20' 
set ylabel 'Average Coverage' font 'Arial,20' 
set yrange[0:1]
set grid ytics lt 0 lc rgb '#7d7d7d'
set ytics 0.2 font 'Arial, 20'
plot 'output/Ave_data_IKEBUCHI_s0.6.txt' using 1:3 with lines lw 2 title '提案手法', 'output/Ave_data_ITO_1.25.txt' using 1:3 with lines dashtype '--' lw 2  title '先行手法'
set output
set terminal pngcairo size 360,492
set output 'graph/mergegraph2.png'
set xlabel 'Elapsed Time'font 'Arial, 20' 
set xtics 40 font 'Arial, 20'
set ylabel 'Average Coverage' font 'Arial,20'
set yrange[0:1]
set grid ytics lt 0 lc rgb '#7d7d7d'
plot'output/Ave_data_IKEBUCHI_s0.6.txt' using 1:2 with lines lw 2 title '提案手法', 'output/Ave_data_ITO_1.25.txt' using 1:2 with lines dashtype '--' lw 2 title '先行手法'
set output
