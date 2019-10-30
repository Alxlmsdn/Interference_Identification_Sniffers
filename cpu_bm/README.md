# CPU Sniffer

Use:
> Make 

To create the executable for this sniffer 


## Execute Sniffer  
Sniffer can be ran by manually running the C executable or using the Python application:
### Sniffer Arguments 
* num. Threads
* Prime Number
* runTime/duration

### Example
> ./cpu 3 10000987 0.75

## Using run_test.py
Running the sniffer using the python program allows results to be plotted or exported
(Ensure *make* has been ran to make the C executable)

### Example
> ./run_test.py --numThreads 3 -i 10000987 -t 0.75

    