# Instruction Cache Latency Sniffer

Use:
> Make 

To create the executable for this sniffer 

## Execute Sniffer  
Sniffer can be ran by manually running the C executable or using the Python application:
### Sniffer Arguments
* Array Size
* runTime/duration

### Example 
> ./icache 100000 2 

## Using run_test.py
Running the sniffer using the python program allows results to be plotted or exported
(Ensure *make* has been ran to make the C executable)

### Example 
> ./run_test.py -s 1000000 -t 2
