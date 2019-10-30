# Network Bandwidth Sniffer

This sniffer uses two programs:
* network_bw
* network_sink

## Network_bw
This program is the main sniffer, and is designed to be executed on the target system

Use:
> Make 

To create the executable for this sniffer 

## Execute Sniffer  
Sniffer can be ran by manually running the C executable or using the Python application:
### Sniffer Arguments 
* URL/IP of reciever program
* Port reciever is listning on
* Length of message to send (bytes)
* runTime/duration

### Example
> ./network_bw 127.0.0.1 55555 10000000 2

## Using run_test.py
Running the sniffer using the python program allows results to be plotted or exported
(Ensure *make* has been ran to make the C executable)

### Example 
> ./run_test.py --ip 127.0.0.1 --port 55555 -s 10000000 -t 5


## network_sink

This program is desined to be executed on a system external to the target system, data packets are sent from the main sniffer program to this network sink.

Use:
> Make networkSink

To create the executable for this sniffer-helper

 ## Excuting network sink

 This program can only be ran using the C executable 
### Network_sink Arguments
* Port to listen on

### Example
> ./networkSink 55555
