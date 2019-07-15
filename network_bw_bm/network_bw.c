/////////////////////////////////////////////////////////////////
// Author: Alexander Lumsden 2019
//      network_bw is the micro benchmark that is responsible for
//      stressing the outgoing network bandwidth of the system.
//      The bm will use a two programs, one will be responisble 
//      for sending a random message to a recieving network sink 
//      program hosted on another system (not the target) the
//      throughput in MB/s will be measured. 
//

#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <cctimer.h>
#include <threadaffinity.h>
#include <pthread.h>



int main(int argc, char *argv[]) {

    if (argc < 4) {
        fprintf(stderr, "Usage: <url/IP> <port> <messageSize>\n");
        return -1;
    }

}