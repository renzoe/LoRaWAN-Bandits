set datafile separator ' '

#plot 'nodeData.txt' using  3:($1==1200?$4:1/0):5 with point pt 7 palette z   # 1 SAMPLES
plot 'nodeData.txt' using  3:($1==12000?$4:1/0):5 with point pt 7 palette z   # 11 SAMPLES
plot 'nodeData.txt' using  3:($1==22800?$4:1/0):5 with point pt 7 palette z   # 20 SAMPLES
plot 'nodeData.txt' using  3:($1==34800?$4:1/0):5 with point pt 7 palette z   # 30 SAMPLES
plot 'nodeData.txt' using  3:($1==48000?$4:1/0):5 with point pt 7 palette z   # 40 SAMPLES
plot 'nodeData.txt' using  3:($1==60000?$4:1/0):5 with point pt 7 palette z   # 50 SAMPLES

plot 'nodeData.txt' using  3:($1==70800?$4:1/0):5 with point pt 7 palette z   # 60 SAMPLES
plot 'nodeData.txt' using  3:($1==82800?$4:1/0):5 with point pt 7 palette z   # 70 SAMPLES
plot 'nodeData.txt' using  3:($1==94800?$4:1/0):5 with point pt 7 palette z   # 80 SAMPLES
plot 'nodeData.txt' using  3:($1==106800?$4:1/0):5 with point pt 7 palette z  # 90 SAMPLES


plot 'nodeData.txt' using  3:($1==118800?$4:1/0):5 with point pt 7 palette z  # 100 SAMPLES


plot 'nodeData.txt' using  3:($1==298800?$4:1/0):5 with point pt 7 palette z # 250 SAMPLES
plot 'nodeData.txt' using  3:($1==598800?$4:1/0):5 with point pt 7 palette z # 500 SAMPLES
plot 'nodeData.txt' using  3:($1==1.1988e+06?$4:1/0):5 with point pt 7 palette z # 1000 SAMPLES

plot 'nodeData.txt' using  3:($1==3.5988e+06 ?$4:1/0):5 with point pt 7 palette z # 3000 SAMPLES



plot 'gwData.txt' using  3:($1==0?$4:1/0):5 with point pt 7 palette z   # GWs  
