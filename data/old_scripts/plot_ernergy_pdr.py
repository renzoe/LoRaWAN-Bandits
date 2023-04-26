#! /usr/bin/env python3

import os

import numpy as np
import matplotlib.pyplot as plt


# Define path to file using os.path.join
#fname = os.path.join("nodeData-2500m.txt")
#fname = os.path.join("nodeData-2500m-bane.txt")
fname = os.path.join("nodeData.txt")
fglobal = os.path.join("globalPerformance.txt")


# Import file to numpy array
node_data = np.loadtxt(fname)

global_perf = np.loadtxt(fglobal)

# 12000 # 11 SAMPLES
# 22800 # 20 SAMPLES
# 34800 # 30 SAMPLES
# 36000 # 31 SAMPLES

periods = 100
time  = 1200 * (periods - 1)


filter = np.asarray([time])

#numpy.asarray([x for x in a if x[1] in filter ])
#https://stackoverflow.com/questions/38910258/python-numpy-filter-two-dimensional-array-by-condition
filtered_data  = node_data[np.in1d(node_data[:, 0], filter)] # Will contain only rows with "time" in first colum 
filtered_data_raw_sf = filtered_data[:,4] # The 5th column contains the Data Rate 


#print(filtered_data.shape)
print(filtered_data_raw_sf.shape)
#print(filtered_data_raw_sf)


hist, bin_edges = np.histogram(filtered_data_raw_sf, bins = range(7))
print(hist)

#https://loratools.nl/#/airtime
# 32 Bytes
# SF7  =  71.94 ms   (25.17 :1)
# SF8  = 133.63 ms   (13.55 :1)
# SF9  = 246.78 ms   (7.34  :1)
# SF10 = 452.61 ms   (4.00  :1)
# SF11 = 987.14 ms   (1.83  :1)
# SF12 = 1,810.43 ms (1     :1)

sf_toa = [1810.43 , 987.14 , 452.61 , 246.78, 133.63, 71.94 ]

raw_toa = hist * sf_toa 
total_packets = np.sum(hist)

print (raw_toa)
raw_toa_sum = np.sum(raw_toa)
print (raw_toa_sum)
print(total_packets)
toa_per_packet = raw_toa_sum/total_packets
print(toa_per_packet)


#print(bin_edges)

#plt.hist(filtered_data_raw_sf, bins=[0, 1, 2, 3,4,5,6])
#plt.show()


#print(global_perf)
filtered_data_for_pdr  = global_perf[np.in1d(global_perf[:, 0], filter)] # Will contain only rows with "time" in first colum  
#filtered_data_raw_pdr = filtered_data[:,4] # The 5th column contains the Data Rate 
print(filtered_data_for_pdr)
#a = global_perf[:,2]/ global_perf[:,1]
single_pdr = filtered_data_for_pdr[:,2]/ filtered_data_for_pdr[:,1]
print(single_pdr)


pdr_div_mean_toa = 1000 * single_pdr/toa_per_packet
print(pdr_div_mean_toa)

print(1000*1/sf_toa[5])
print(1000*1/sf_toa[0])
