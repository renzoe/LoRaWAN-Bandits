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



def metric_pdr(row):

    pdr = row[2]/row[1]
    return pdr


def metric_pdr_calculus(fglobal):
    print("File: " , fglobal)
    # Import file to numpy array
    global_perf = np.loadtxt(fglobal)

    period_duration = 1200
    period_min = 91
    period_max = 100
    time_min  = period_duration * (period_min - 1)
    time_max  = period_duration * (period_max - 1)


#https://datascienceparichay.com/article/filter-numpy-array-with-examples/
    filtered_arr = global_perf[(global_perf >= time_min) & (global_perf <= time_max)]
#print("rows to filter: ")
#print(filtered_arr)
    filtered_data  = global_perf[np.in1d(global_perf[:, 0], filtered_arr)] # Will contain only rows with "filtered_arr" in first colum 
    #print(filtered_data)

    pdr_array = np.apply_along_axis(metric_pdr, 1, filtered_data)
    print("array_pdr: ", pdr_array)


    print ("mean: "   , np.mean(pdr_array, dtype=np.float32))
#print ("std: " , np.std(pdr_array, dtype=np.float64))
    print ("std: " , np.std(pdr_array, dtype=np.float32))
    print("\n")


#print ("std: " , np.std(filtered_data[:, 2], dtype=np.float64))
    #print ("std: " , np.std(filtered_data[:, 2], dtype=np.float32))



sys.stdout = Logger()
print("----------------------------------------------------------")
print(datetime.now(tz=None))

# Define path to file using os.path.join
fglobal_1 = os.path.join("./02-BanditPDR-b=0-p=0.1/globalPerformance.txt")
fglobal_2 = os.path.join("./02-BanditPDR-b=15-p=0.1/globalPerformance.txt")
fglobal_3 = os.path.join("./03-BanditEnergyPDR-b=15-p=0.1/globalPerformance.txt")



metric_pdr_calculus(fglobal_1)
metric_pdr_calculus(fglobal_2)
metric_pdr_calculus(fglobal_3)


