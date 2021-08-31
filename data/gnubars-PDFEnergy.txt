#https://stackoverflow.com/questions/12235683/how-to-display-a-stacked-barchart-in-gnuplot
set style data histograms
set style histogram rowstacked
set boxwidth 1 relative
set style fill solid 1.0 # border -1
set yrange [0:1000]
set datafile separator " "
plot 'nodeData.txt' using 1 t "Received", '' using 5:xticlabels(1) t "Interfered"  , '' using 6:xticlabels(1) t "NO MORE RECEIVERS"  , '' using 7:xticlabels(1) t "UNDER SENSITIVITY" , '' using 8:xticlabels(1) t "LOST BECAUSE TX"       

