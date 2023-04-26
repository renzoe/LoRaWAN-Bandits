#! /usr/bin/env python3

#Copyright (c) 2022 Renzo E. Navas
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:

#The above copyright notice and this permission notice shall be included in all
#copies or substantial portions of the Software.

#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#SOFTWARE.

from array import array
import os

import numpy as np
import matplotlib.pyplot as plt
import sys
import time
from datetime import datetime
from pathlib import Path

#https://stackoverflow.com/questions/14906764/how-to-redirect-stdout-to-both-file-and-console-with-scripting
class Logger(object):
    def __init__(self):
        self.terminal = sys.stdout
        self.log = open("results-" + Path(__file__).stem + ".log", "a")
   
    def write(self, message):
        self.terminal.write(message)
        self.log.write(message)  

    def flush(self):
        # this flush method is needed for python 3 compatibility.
        # this handles the flush command by doing nothing.
        # you might want to specify some extra behavior here.
        pass  

#dBm to Watt Conversion Table http://www.cpcstech.com/dbm-to-watt-conversion-information.htm
#   dbm =  0     1     2     3     4     5   6  7  8 9 10 11 12 13 14
#db-tomw = [0 , 1.3 , 1.6 , 2.0 , 2.5 , 3.2 , 4 ,5, 6,8,10,13,16,20,25]
# https://github.com/yadox666/dBm2mW/blob/master/dBm2mW.py
# Function to convert from mW to dBm
def mW2dBm(mW):
    return 10.*log10(mW)

# Function to convert from dBm to mW
def dBm2mW(dBm):
    return 10**((dBm)/10.)


#https://loratools.nl/#/airtime
# 32 Bytes
# SF7  =  71.94 ms   (25.17 :1)
# SF8  = 133.63 ms   (13.55 :1)
# SF9  = 246.78 ms   (7.34  :1)
# SF10 = 452.61 ms   (4.00  :1)
# SF11 = 987.14 ms   (1.83  :1)
# SF12 = 1,810.43 ms (1     :1)
# https://stackoverflow.com/questions/50997928/typeerror-only-integer-scalar-arrays-can-be-converted-to-a-scalar-index-with-1d
def sf2toafor32B(sf):
    sf_toa_32B = [1810.43 , 987.14 , 452.61 , 246.78, 133.63, 71.94 ]
    return  np.array(sf_toa_32B)[sf.astype(int)]



def energy_one_period(node_data, period, period_duration):
    time  = period_duration * (period - 1)

    filter = np.asarray([time])

#numpy.asarray([x for x in a if x[1] in filter ])
#https://stackoverflow.com/questions/38910258/python-numpy-filter-two-dimensional-array-by-condition
    filtered_data  = node_data[np.in1d(node_data[:, 0], filter)] # Will contain only rows with "time" in first colum 
    filtered_data_raw_sf = filtered_data[:,4].astype(int) # The 5th column contains the Data Rate 
    filtered_data_raw_toa =  sf2toafor32B(filtered_data[:,4])
#print(filtered_data_raw_toa)


    filtered_data_raw_dBm = filtered_data[:,5] # The 6th column contains the dBm
    filtered_data_raw_mw = dBm2mW(filtered_data[:,5]) # The 6th column contains the dBm
#print(filtered_data_raw_mw)


    mwxms = filtered_data_raw_mw * filtered_data_raw_toa
    total_mwxms = np.sum(mwxms)
#1 watt-second equals 1 Joule! 
    total_mwxms_WS =  total_mwxms / (1000*1000)
    #print ("Period", period, " Joules: ", total_mwxms_WS)
    return total_mwxms_WS




def metric_joules_calculus(filename, period_min, period_max, period_duration):
    print("File: " , filename)
    # Import file to numpy array
    node_data = np.loadtxt(filename)
    samples = np.empty(shape=[0, 1])

    for period in range(period_min, period_max + 1 ):
        samples = np.append(samples, energy_one_period(node_data, period, period_duration))


    #print("array_joules: ", samples)
    #print ("mean: " , np.mean(samples, dtype=np.float32))
    #print ("std: "  , np.std(samples, dtype=np.float32))
    #print("\n")
    return samples



def metric_pdtotal(fglobal, period_min, period_max, period_duration):
    print("File: " , fglobal)
    # Import file to numpy array
    global_perf = np.loadtxt(fglobal)

    time_min  = period_duration * (period_min - 1)
    time_max  = period_duration * (period_max - 1)


#https://datascienceparichay.com/article/filter-numpy-array-with-examples/
    filtered_arr = global_perf[(global_perf >= time_min) & (global_perf <= time_max)]
    filtered_data  = global_perf[np.in1d(global_perf[:, 0], filtered_arr)] # Will contain only rows with "filtered_arr" in first colum 
    #print(filtered_data)

    #pdr_array = np.apply_along_axis(metric_pdr, 1, filtered_data)
    pdtotal_array  = filtered_data[:,2]
    #print("pdtotal_array: ", pdtotal_array)
    return  pdtotal_array




def unec_mj(fnodes_1, fglobal_1, period_min, period_max, period_duration):
    total_energy            = metric_joules_calculus(fnodes_1, period_min, period_max, period_duration)
    total_packets_delivered = metric_pdtotal(fglobal_1, period_min, period_max, period_duration)

#NEC is energy to deliver one packet, here we set in miliJoules
    nec_mj =  total_energy * 1000 / total_packets_delivered

    print( "uNEC mJ: ",  nec_mj)
    return nec_mj


def mean_and_sd(nparray):
    print ("mean: " , np.mean(nparray, dtype=np.float32))
    print ("std: "  , np.std(nparray, dtype=np.float32))
    print ("\n")


sys.stdout = Logger()
print("----------------------------------------------------------")
print(datetime.now(tz=None))

# Define path to file using os.path.join
fnodes_1 = os.path.join("./01-LegacyADR/nodeData.txt")
fnodes_2 = os.path.join("./02-BanditPDR/nodeData.txt")
fnodes_3 = os.path.join("./03-BanditEnergyPDR/nodeData.txt")

fglobal_1 = os.path.join("./01-LegacyADR/globalPerformance.txt")
fglobal_2 = os.path.join("./02-BanditPDR/globalPerformance.txt")
fglobal_3 = os.path.join("./03-BanditEnergyPDR/globalPerformance.txt")


period_duration = 1200
period_min = 91
period_max = 100


nec_mj_1 = unec_mj(fnodes_1, fglobal_1, period_min, period_max, period_duration)
mean_and_sd(nec_mj_1)

nec_mj_2 = unec_mj(fnodes_2, fglobal_2, period_min, period_max, period_duration)
mean_and_sd(nec_mj_2)

nec_mj_3 = unec_mj(fnodes_3, fglobal_3, period_min, period_max, period_duration)
mean_and_sd(nec_mj_3)







