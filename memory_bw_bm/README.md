# Memory Bandwidth Sniffer

Use:
> Make 

To create the executable for this sniffer 

## Execute Sniffer  
Sniffer can be ran by manually running the C executable or using the Python application:
### Sniffer Arguments 
* Array size (At least x4 the size of cache)
* runTime/duration

### Example 
> ./memory_bw 10000000 10

## Using run_test.py
Running the sniffer using the python program allows results to be plotted or exported
(Ensure *make* has been ran to make the C executable)

### Example 
> ./run_test.py -s 10000000 -t 10 

    

    
