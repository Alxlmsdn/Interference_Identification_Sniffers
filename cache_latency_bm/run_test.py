#! /usr/bin/env python

# Author: Alexander Lumsden 2019 
# orignal code from: Christopher Celio 2010
#   
# Python script to run cache latency test, collect data, and generate
# plots


# change plot size, font sizes for opitmal poster printing
PLOT_POSTER=1

# readjust the Python import path to accept our own common file
import sys
sys.path.append('../common/')
import os
import collections

# Put all of the common functions into ccbench, and make run_test.py as small
# as possible. run_test.py should basically only hold the graphing code, since
# that is custom to the benchmark.
import ccbench      
#import ccprocstats

APP = "caches"
BASE_DIR="./"
REPORT_DIR="../Reports/"
#for K8's testing----------
#REPORT_DIR="/data/Reports/"
PLOT_DIR="../Plots/"
DEFAULT_REPORT_NAME = "report.txt"

variables = (
    "level",
    "thread",
    "totalTime",
    "latency"
    )
 
# Sometimes we run on remote platforms without access to matplotlib.  In this
# case, don't import the graphing functions and don't run the graphing code. 
# We'll just graph from the report file at a later date on another machine.
NOPLOT = False
try:
    import matplotlib
    matplotlib.use('PDF') # must be called immediately, and before import pylab
                          # sets the back-end for matplotlib
    import matplotlib.pyplot as plt
    import numpy as np
    import pylab
except:
    NOPLOT = True
    print("Failure to import {matplotlib/numpy/pylab}. Graphing turned off.")
    

# 1. Parses input file.
# 2. Runs benchmark (many times) and records results in results file.
# 3. Parses results file for data (can also play back recording using NORUN option).
# 4. Graphs results and outputs to .pdf file.
def main(args=None, report_dir=None):
    global REPORT_DIR
    global PLOT_DIR
    # handle CLI options
    if (report_dir != None):
        REPORT_DIR = report_dir
        PLOT_DIR = report_dir
    ccbench.controller(args)  
    #handle default/cli args app
    app_bin = BASE_DIR + APP
    #input_filename = BASE_DIR +  DEFAULT_INPUT_NAME
    report_filename = REPORT_DIR + ccbench.getReportFileName(APP, REPORT_DIR)   
    report_csv_filename = REPORT_DIR + ccbench.getReportFileName(APP, REPORT_DIR, ".csv")

    # 1. Parse inputs.txt file.

    # Build up the arguments list for each invocation of the benchmark.
    # This is done here, instead of in ccbench.py, because this is custom to each app.
    app_args_list = []
    if (ccbench.CONFIG != 'none'):
        ccbench.parseConfigFile(APP)
        
    if (ccbench.LEVEL1 != "0"):
        app_args_list.append("L1 " + ccbench.LEVEL1 + " " + ccbench.ITERATIONS + "00 " + ccbench.MIN_RUN_TIME)
    if (ccbench.LEVEL2 != "0"):
        app_args_list.append("L2 " + ccbench.LEVEL2 + " " + ccbench.ITERATIONS + "0 " + ccbench.MIN_RUN_TIME)
    if (ccbench.LEVEL3 != "0"):
        app_args_list.append("L3 " + ccbench.LEVEL3 + " " + ccbench.ITERATIONS + " " + ccbench.MIN_RUN_TIME)

    # 2. Execute the benchmark and write to the report file.
    ccbench.runBenchmark(app_bin, app_args_list, report_filename)
        
    # 3. Extract Data from the report file.
    data = ccbench.readReportFile(report_filename, variables)
    
    # 4. Plot the Data
    #print data
    if (not ccbench.NOPLOT):
        if PLOT_POSTER:
            fig = plt.figure(figsize=(5,3.5))
            font = {#'family' : 'normal',
                #'weight' : 'bold',
                'size'   : 8}
            matplotlib.rc('font', **font)
            fig.subplots_adjust(top=0.94, bottom=0.14, left=0.1, right=0.96,wspace=0, hspace=0)
        else:
            fig = plt.figure(figsize=(9,5.5))
            fig.subplots_adjust(top=0.95, bottom=0.12, left=0.07, right=0.97,wspace=0, hspace=0)
        
        p1 = fig.add_subplot(1,1,1)

        print("Plotting time...")

    sets = collections.OrderedDict()
    if  (ccbench.NOPLOT):
        csv_file = open(report_csv_filename, "a+")
        header = "totalTime"
        data_list = []

    for i in range(len(data["thread"])):
        level = data["level"][i]
        thread = data["thread"][i]
        if level not in sets:
            sets[level] = collections.OrderedDict()

        if thread in sets.get(level):
                sets[level][thread][0].append(data["totalTime"][i])
                sets[level][thread][1].append(data["latency"][i])
        else:
            sets[level][thread] = [[data["totalTime"][i]],[data["latency"][i]]]
    commas = 0
    for level, threads in sets.items():
        columns = len(sets) * len(threads)
        for thread, times in threads.items():
            if (ccbench.NOPLOT):
                header += ",{}_{}".format(level, thread)
                for i in range(len(times[0])):
                    data_list.append(times[0][i] + "," + (","*commas) + times[1][i] + ("," *(columns-1 - commas))+"\n")
                commas += 1
            else:
                plot_label = 'Thread ' + thread
                p1.plot(
                    [float(i) for i in times[0]],
                    [float(i) for i in times[1]],
                    label= plot_label
                )
    if (ccbench.NOPLOT):
        csv_file.write(header+"\n")
        csv_file.writelines(data_list)
        csv_file.close()
        print("Saved data in CSV -> " + report_csv_filename)
    else:            
        plt.legend(loc='upper left')

        p1.set_yscale('log')
        plt.ylabel("latency (ns)")
        plt.xlabel('Time (s)')
        plt.ylim((1, 320))
        
    
        ytick_range = [1,2,4,8,16,32,64,128,256] # in ns / iteration
        ytick_names = ['1','2','4','8','16','32','64','128','256']
        
        p1.yaxis.set_major_locator( plt.NullLocator() )
        p1.yaxis.set_minor_locator( plt.NullLocator() )
        plt.yticks(ytick_range,ytick_names)
        

        if (ccbench.PLOT_FILENAME == "none"):
            filename = PLOT_DIR + ccbench.generatePlotFileName(APP)
        else:
            # Pull out the filename path from the full path.
            # This allows us to pull out the requested filename from the path presented
            # (since we always write reports to the report directory, etc.). However, it
            # allows the user to use tab-completion to specify the exact reportfile he
            # wants to use.
            filename = PLOT_DIR + os.path.basename(ccbench.PLOT_FILENAME)
            filename = os.path.splitext(filename)[0]
            
        plt.savefig(filename)
        print("Used report filename             : " + report_filename)
        print("Finished Plotting, saved as file : " + filename + ".pdf")



                
#This idiom means the below code only runs when executed from the command line
if __name__ == '__main__':
  main()
  #print 'finished with main from CLI'

