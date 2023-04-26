
#show terminal

set terminal pdf size 20,20





#set term postscript eps enhanced color
set output "single-gw-scatter.pdf"



#set terminal pngcairo size 800,600 background rgb '#bbbbbb'
#set output 'foobar.png'


set datafile separator ' '


#https://stackoverflow.com/questions/23927580/removing-blank-gap-in-gnuplot-multiplot/23936306#23936306
set multiplot layout 3,3 \
              margins 0.1,0.98,0.1,0.98 \
              spacing 0.08,0.08


#plot 'nodeData.txt' using  3:($1==1200?$4:1/0):5 with point pt 7 palette z   # 1 SAMPLES

set ylabel 'holis'
plot './01-LegacyADR/nodeData.txt' using  3:($1==12000?$4:1/0):5 with point pt 7 palette z   # 11 SAMPLES
plot './01-LegacyADR/nodeData.txt' using  3:($1==60000?$4:1/0):5 with point pt 7 palette z   # 50 SAMPLES
plot './01-LegacyADR/nodeData.txt' using  3:($1==118800?$4:1/0):5 with point pt 7 palette z   # 100 SAMPLES

unset ylabel
#unset xtics
#unset ytics
plot './02-BanditPDR/nodeData.txt' using  3:($1==12000?$4:1/0):5 with point pt 7 palette z   # 11 SAMPLES
plot './02-BanditPDR/nodeData.txt' using  3:($1==60000?$4:1/0):5 with point pt 7 palette z   # 50 SAMPLES
plot './02-BanditPDR/nodeData.txt' using  3:($1==118800?$4:1/0):5 with point pt 7 palette z   # 100 SAMPLES

plot './03-BanditEnergyPDR/nodeData.txt' using  3:($1==12000?$4:1/0):5 with point pt 7 palette z   # 11 SAMPLES
plot './03-BanditEnergyPDR/nodeData.txt' using  3:($1==60000?$4:1/0):5 with point pt 7 palette z   # 50 SAMPLES
plot './03-BanditEnergyPDR/nodeData.txt' using  3:($1==118800?$4:1/0):5 with point pt 7 palette z   # 100 SAMPLES



unset multiplot

