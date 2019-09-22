Running network_bw:
    url/ip port messageLength runTime
    ./network_bw 127.0.0.1 55555 10000000 2

for tablet sink:
	./network_bw 192.168.1.76 55555 1000000 5

using run_test.py:
    ./run_test.py --ip 127.0.0.1 --port 55555 -s 10000000 -t 5

This benchmark also uses a second program that should run on a seperate machine to recieve the message being
sent from the main benchmark:

Running ./networkSink:
(first make the executable with 'make networkSink')
    port
    ./networkSink 55555
