#https://stackoverflow.com/questions/12235683/how-to-display-a-stacked-barchart-in-gnuplot

set terminal pdf
set output "single-gw-bars.pdf"




set style data histograms
set style histogram rowstacked
set boxwidth 1 relative
set style fill solid 0.75  border -1 # http://gnuplot.sourceforge.net/demo/fillstyle.html

set yrange [0:1000]
set ytics 0,250
set datafile separator " "


set multiplot # layout 3,1

# https://stackoverflow.com/questions/63850032/gnuplot-multiplot-equal-height-of-each-plot

myBottomMargin = 0.14
myPlotHeight = (1. - myBottomMargin)/3


#set key reverse Left outside

#set key reverse Left above
#set key default
#set key bottom right
#set key at screen 0.5,screen 0.1 #for example

unset key
#unset xtics


#############################################
#### FIRST PLOT
set origin 0, myBottomMargin
set size 1, myPlotHeight-0.0
set bmargin 0
#set title "Bandit Energy-PDR" 


set xlabel 'Simulated Period # (20min per Period)'
set ylabel '# Packets' offset 2
set xtics 0,10
#set label "Bandit Energy-PDR" at graph 0.05,0.2 left front

# https://stackoverflow.com/questions/15362440/boxed-label-with-background-color
LABEL = "(c) Bandit Energy-PDR"
set obj 10 rect front at graph 0.21,0.2  size char strlen(LABEL), char 1.5 fc rgb "white"
set label 10 LABEL at graph 0.05,0.2 left front

plot './03-BanditEnergyPDR/phyPerformance.txt' using 4 t "Received", '' using 5 t "Interfered"  ,  '' using 7 t "Under Sensitivity" , '' using 8 t "Lost Because TX"
unset label


unset xlabel
unset xtics
unset key

#############################################
#### SECOND PLOT
set origin 0, myBottomMargin+myPlotHeight

#set title "Bandit PDR" 
set ylabel '# Packets' offset 2
set label "(b) Bandit PDR" at graph 0.05,0.2 left front
plot './02-BanditPDR/phyPerformance.txt' using 4 t "Received", '' using 5 t "Interfered"  , '' using 7 t "Under Sensitivity" , '' using 8 t "Lost Because TX"
unset label



#############################################
#### THIRD PLOT

set origin 0, myBottomMargin+2*myPlotHeight
#set title "LoRaWAN ADR"
#set key reverse Left above
set key reverse Left opaque  bottom right #title "(a) LoRaWAN ADR"

set ylabel '# Packets' offset 2
set label "(a) LoRaWAN ADR" at graph 0.05,0.2 left front
plot './01-LegacyADR/phyPerformance.txt' using 4 t "Received", '' using 5 t "Interfered"  ,  '' using 7 t "Under Sensitivity" , '' using 8 t "Lost Because TX"
unset label
#plot '01-phyPerformance.dat' using 4 t "Received", '' using 5:xticlabels(1) t "Interfered"  , '' using 6:xticlabels(1) t "NO MORE RECEIVERS"  , '' using 7:xticlabels(1) t "UNDER SENSITIVITY" , '' using 8:xticlabels(1) t "LOST BECAUSE TX"


unset multiplot
