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

#define _GNU_SOURCE
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
#include <sched.h>
//#include <sys/sysinfo.h>

#define DEBUG

#define STREAM_ELEMENTS double

void memory_bw_bm(void *);

static STREAM_ELEMENTS *a, *b, *c;
static double average_times[4] = {0};
static double num_bytes[4];

int main(int argc, char* argv[]) {

    if (argc != 4) {
        fprintf(stderr, "Usage: <number of threads> <array size> <number of iterations>\n");
        return 1;
    }
    //uint32_t num_threads = atoi(argv[1]);
    uint32_t array_size = atoi(argv[2]);
    uint32_t iterations = atoi(argv[3]);

    a = (STREAM_ELEMENTS *) malloc(sizeof(STREAM_ELEMENTS) * array_size);
    b = (STREAM_ELEMENTS *) malloc(sizeof(STREAM_ELEMENTS) * array_size);
    c = (STREAM_ELEMENTS *) malloc(sizeof(STREAM_ELEMENTS) * array_size);

    num_bytes[0] = 2 * sizeof(STREAM_ELEMENTS) * array_size;
    num_bytes[1] = 2 * sizeof(STREAM_ELEMENTS) * array_size;
    num_bytes[2] = 3 * sizeof(STREAM_ELEMENTS) * array_size;
    num_bytes[3] = 3 * sizeof(STREAM_ELEMENTS) * array_size;

    int num_cores = sysconf(_SC_NPROCESSORS_CONF);

    cpu_thread_t** threads = setThreads(memory_bw_bm, num_cores);

#ifdef DEBUG
    fprintf(stdout, "number of threads: %d\n", num_cores);
#endif

    for(uint32_t i = 0; i < num_cores; i++) {
        pthread_join(*(threads[i]->thread), NULL);
    }
#ifdef DEBUG
    fprintf(stdout, "....Finished....\n");
#endif
    return 0;
}

void memory_bw_bm(void* tid) {
    uint32_t id = *(uint32_t*) tid;
    fprintf(stdout, "done %d\n", id);
}
