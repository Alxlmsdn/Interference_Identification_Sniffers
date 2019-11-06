#! /usr/bin/env python

# Author: Christopher Celio
# Date  : 2011 May 15
#   
# All common Python routines needed by the run_tests.py go in here.  In
# particular, command-line argument parsing, input file parsing, running the
# benchmarks, storing results to an output file, and parsing the output file
# for graphing all goes in here.
#
# The actual graphing code goes elsewhere, since that gets tailored to the
# individual benchmark.


from subprocess import Popen # now we can make system calls
from subprocess import PIPE  # now we can make system calls
import optparse    # parse CLI options
import os

from datetime import datetime 


# This function takes Bash commands and returns them
def runBash(cmd):
    p = Popen(cmd, shell=True, stdout=PIPE)
    out = p.stdout.read().strip()
    return out #This is the stdout from the shell command
 
                                       
# This function takes in a line from a file, and a search string, and returns 
# the string between the nearest [,] delineators that follow the str
#   thus,       "Cycles=[1234]" returns "1234"
def parseValueFromLine(line, str):
    value = line[line.find(str) + len(str) :]
    srt_idx = value.find("[") + 1
    end_idx = value.find("]")
    value = value[srt_idx : end_idx]
    return value.strip()
                                      
#Function to control option parsing in Python
def controller(args=None):
    global NOPLOT
    global REPORT_FILENAME
    global PLOT_FILENAME
    global LEVEL1
    global LEVEL2
    global LEVEL3
    global ITERATIONS
    global MIN_RUN_TIME
    global THREADS
    global SIZE
    global IP
    global PORT
    global CONFIG
    global FILE_PATH

    #create instance of Option Parser Module, included in Standard Library
    p = optparse.OptionParser(description='CLI Controller for memory system u-kernels',
                  prog='./run_test.py',
                  version='cc-ukernel 0.6',
                  usage= '%prog [option]')
    p.add_option('--outfilename','-o', dest="outfilename",
                  help='Filename for the generated output plot. The file is always placed in the \'plots\' directory. If directory name is provided as part of the \'outfilename\', it will be automatically stripped out (allows you to easily specify an existing filename).',
                  default = "none")
    p.add_option('--reportfilename','-r', dest="reportfilename",
                  help='Filename to use for the report file. Data from runs are stored in this file. Also, plots are generated from the data in this file. The file is always placed in the \'reports\' directory. If the directory name is provided as part of the \'reportfilename\', it will be automatically stripped out (allows you to easily specify the existing report you want to use).',
                  default = "none")
    p.add_option('--noplot','-g', dest="noplot", action="store_true",
                  help='Skips plotting ukernels (runs ukernels and saves to results.txt file).',
                  default = False)
    p.add_option('--level1', '-1', dest = 'level1',
                  help='size of level 1 data cache (Bytes)',
                  default = "0")
    p.add_option('--level2', '-2', dest = 'level2',
                  help='size of level 2 data cache (Bytes)',
                  default = "0")
    p.add_option('--level3', '-3', dest = 'level3',
                  help='size of level 3 data cache (Bytes)',
                  default = "0")
    p.add_option('--iterations', '-i', dest = 'iterations',
                  help='number of iterations',
                  default = "5000000")
    p.add_option('--minRunTime', '-t', dest = 'minRunTime',
                  help='minimum length of runtime for cache test',
                  default = "0.75")
    p.add_option('--numThreads', dest = 'numThreads',
                  help='number of threads to create',
                  default = "1") 
    p.add_option('--size', '-s', dest = 'size',
                  help='size of array in elements',
                  default= "10000000")   
    p.add_option('--ip', dest = 'ip',
                  help='ip address to connect to',
                  default= "127.0.0.1")
    p.add_option('--port', dest = 'port',
                  help='port number',
                  default= "5555") 
    p.add_option('--configFile', '-c', dest = 'config',
                  help='input config file',
                  default= "none") 
    p.add_option('--file_path', dest = 'file_path',
                  help='where the temp files should be written',
                  default= "") 

    #option Handling passes correct parameter to runBash 
    if (args == None):
        options, arguments = p.parse_args() 
    else:
        options, arguments = p.parse_args(args=args)
    #INPUT_TYPE     = options.input_type
    REPORT_FILENAME= options.reportfilename
    PLOT_FILENAME  = options.outfilename
    NOPLOT         = options.noplot
    LEVEL1         = options.level1
    LEVEL2         = options.level2
    LEVEL3         = options.level3
    ITERATIONS     = options.iterations
    MIN_RUN_TIME   = options.minRunTime
    THREADS        = options.numThreads
    SIZE           = options.size
    IP             = options.ip
    PORT           = options.port
    CONFIG         = options.config
    FILE_PATH      = options.file_path

def parseConfigFile(app):
    input_file = open(CONFIG).readlines() 

    input_finished = False

    for line in input_file:

        if input_finished:
            continue

        idx = (line.strip()).find("#")
        if idx == 0:
            continue

        if (app in line):
            params = line.split()
            controller(params[1:])
            input_finished = True
    return

         

# app_args is a list of strings, where each string corresponds to a single run to the application binary
#def runBenchmark(app_bin, app_args_list, inputs, input_variables, report_filename):
def runBenchmark(app_bin, app_args_list, report_filename):
     
    import sys
                                        
    # Execute Test
    t = datetime.now()
    time_str = t.strftime("%Y-%m-%d %H:%M:%S")
                 
    for app_args in app_args_list:
        print("" + app_bin + " " + app_args + " >> " + report_filename)
        value = runBash("" + app_bin + " " + app_args + " >> " + report_filename)
        if value != "": 
            print(value)



# returns data from the report file in dictionary form (I think)
def readReportFile(report_filename, variables):

    # 3. Extract Data
    file = open(report_filename).readlines() #open the file
        
    # "data" is a 1D dictionary, indexed by Variable.
    data = {}  # Ordered Dict not in Python 2.6 :(


    #initialize arrays
    for variable in variables:
        data[variable] = []
        
    #one program run per line
    #App:[stencil3],NumThreads:[1],AppSizeArg:[32],Time(s):[55.000000]
    for line in file:
        for variable in variables:

            #ignore commented out lines (first char is '#')
            idx = (line.strip()).find("#")
            if idx == 0:
                continue

            #special variables for the graph code
            #i.e., how many data points exist per set
            idx = (line.strip()).find("@")
            if idx == 0:
                if variable in line:
                    data[variable].append(parseValueFromLine(line, variable))

            # (parse for "variable=[1337]")
            # initialize an element in the data matrix to be an array
            if variable in line:
                data[variable].append(parseValueFromLine(line, variable))


    return data
 

def getReportFileName(app_str, report_dir_path, file_type=".txt"):
    if (REPORT_FILENAME == "none"): 
        report_filename = generateReportFileName(app_str, file_type)
    else:   
        report_filename = os.path.basename(REPORT_FILENAME)
    return report_filename


def generateReportFileName(app_str, file_type):
    t = datetime.now()
    #time_str = t.strftime("%Y-%m-%d %H:%M:%S")
    time_str = t.strftime("%Y-%m-%d_%Hh%Mm%Ss")
    return "reportfile_" + app_str + "_" + time_str + file_type

def generatePlotFileName(app_str):
    t = datetime.now()
    time_str = t.strftime("%Y-%m-%d_%Hh%Mm%Ss")
    return "plot_" + app_str + "_" + time_str



                
#This idiom means the below code only runs when executed from the command line
if __name__ == '__main__':
  print('--Error: ccbench.py is an include file--')

