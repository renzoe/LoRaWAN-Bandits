#! /usr/bin/env python3

import os

import numpy as np
import matplotlib.pyplot as plt


# Define path to file using os.path.join
fname = os.path.join("nodeData-2500m.txt")
#fname = os.path.join("nodeData-2500m-bane.txt")
fname = os.path.join("nodeData.txt")

# Import file to numpy array
node_data = np.loadtxt(fname)

# 12000 # 11 SAMPLES
# 22800 # 20 SAMPLES
# 34800 # 30 SAMPLES
# 36000 # 31 SAMPLES

periods = 60
time  = 1200 * (periods - 1)


filter = np.asarray([time])

#numpy.asarray([x for x in a if x[1] in filter ])
#https://stackoverflow.com/questions/38910258/python-numpy-filter-two-dimensional-array-by-condition
filtered_data  = node_data[np.in1d(node_data[:, 0], filter)] # Will contain only rows with 36000 in first colums  

filtered_data_raw_sf = filtered_data[:,4] # The 5th column contains the Data Rate 


#print(filtered_data.shape)
print(filtered_data_raw_sf.shape)
print(filtered_data_raw_sf)


hist, bin_edges = np.histogram(filtered_data_raw_sf, bins = range(7))
print(hist)
print(bin_edges)

plt.hist(filtered_data_raw_sf, bins=[0, 1, 2, 3,4,5,6])
plt.show()

