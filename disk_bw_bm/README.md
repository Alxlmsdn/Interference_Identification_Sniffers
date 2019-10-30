# Disk Bandwidth Sniffer

Use:
> Make 

To create the executable for this sniffer 

## Execute Sniffer  
Sniffer can be ran by manually running the C executable or using the Python application:
### Sniffer Arguments 
* runTime/duration
* Num. bytes to read/write (0 means use the default file block size)
* File path to write temporary files (optional)

### Example 
> /disk_bw 0.75 0 /data/

## Using run_test.py
Running the sniffer using the python program allows results to be plotted or exported
(Ensure *make* has been ran to make the C executable)

### Example 
> ./run_test.py -t 0.75 -s 0 --file_path /data/


    
