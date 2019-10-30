# Cache Latency Sniffer

Use:
> Make 

To create the executable for this sniffer 

## Execute Sniffer  
Sniffer can be ran by manually running the C executable or using the Python application:
### Sniffer Arguments 
* level 
* sizeOfCacheLevel
* num. iterations 
* runTime/duration 

### Example
> ./caches L1 32768 67108864 2

## Using run_test.py
Running the sniffer using the python program allows results to be plotted or exported
(Ensure *make* has been ran to make the C executable)

### Example 
> ./run_test.py -1 32768 -2 3145728 -i 67108864 -t 0.75
    
    

    
