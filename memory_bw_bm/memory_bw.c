/////////////////////////////////////////////////////////////////
// Author: Alexander Lumsden 2019
//      memory_bw.c is responsible for running the micro-
//      benchmark that will determine the current bandwidth
//      of the RAM of the machine, uses the STREAM metrics 
//      that measures the number of bytes written and bytes
//      read. The benchmark will be multithreaded to determine
//      the bandwidth of each core of the machine. Runtime 
//      paramaters can also make this program suitable for 
//      determining the bandwidth of cache levels. 
//


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <cctimer.h>
#include <pthread.h>
//#include <sys/sysinfo.h>

#define STREAM_ELEMENTS double

static STREAM_ELEMENTS a,b,c;
static double average_times[4] = {0};
static double num_bytes[4];

int main(int argc, char* argv[]) {
    //cctime_t first = cc_get_seconds(0);
    //cctime_t second = cc_get_seconds(0);
    //while((second = cc_get_seconds(0)) == first);
    //fprintf(stdout, "first: %f\n second: %f\n difference: %f", first, second, (second - first) * 1e3);
    //return 0;
    if (argc != 4) {
        fprintf(stderr, "Usage: <number of threads> <array size> <number of iterations>");
        return 1;
    }
    uint32_t num_threads = atoi(argv[1]);
    uint32_t array_size = atoi(argv[2]);
    uint32_t iterations = atoi(argv[3]);

    int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
    fprintf(stdout, "number of cores: %d\n", numCPU);

    return 0;

}
