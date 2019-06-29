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
#include <pthread.h>
#include <sched.h>
//#include <sys/sysinfo.h>

#define DEBUG

#define STREAM_ELEMENTS double

void* setAffinity();

static STREAM_ELEMENTS *a, *b, *c;
static double average_times[4] = {0};
static double num_bytes[4];

int main(int argc, char* argv[]) {
    //cctime_t first = cc_get_seconds(0);
    //cctime_t second = cc_get_seconds(0);
    //while((second = cc_get_seconds(0)) == first);
    //fprintf(stdout, "first: %f\n second: %f\n difference: %f", first, second, (second - first) * 1e3);
    //return 0;
    if (argc != 4) {
        fprintf(stderr, "Usage: <number of threads> <array size> <number of iterations>\n");
        return 1;
    }
    uint32_t num_threads = atoi(argv[1]);
    uint32_t array_size = atoi(argv[2]);
    uint32_t iterations = atoi(argv[3]);

    a = (STREAM_ELEMENTS *) malloc(sizeof(STREAM_ELEMENTS) * array_size);
    b = (STREAM_ELEMENTS *) malloc(sizeof(STREAM_ELEMENTS) * array_size);
    c = (STREAM_ELEMENTS *) malloc(sizeof(STREAM_ELEMENTS) * array_size);

    num_bytes[0] = 2 * sizeof(STREAM_ELEMENTS) * array_size;
    num_bytes[1] = 2 * sizeof(STREAM_ELEMENTS) * array_size;
    num_bytes[2] = 3 * sizeof(STREAM_ELEMENTS) * array_size;
    num_bytes[3] = 3 * sizeof(STREAM_ELEMENTS) * array_size;

    int num_cores_available = sysconf(_SC_NPROCESSORS_ONLN);
    int num_cores_total = sysconf(_SC_NPROCESSORS_CONF);

#ifdef DEBUG
    fprintf(stdout, "number of cores available: %d, from total cores: %d", num_cores_available, num_cores_total);
#endif

    pthread_t threads[num_cores_total];
    uint32_t thread_id[num_cores_total];

    for (uint32_t i = 0 ; i < num_cores_total; i ++) {
        thread_id[i] = i;
        pthread_create(&threads[i], NULL, setAffinity, &thread_id[i]);
    }
    for(uint32_t i = 0; i < num_cores_total; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}


void* setAffinity(int* id) {
    uint32_t thread_id = *(uint32_t *) id;
    pthread_t thread = pthread_self();
    cpu_set_t* cpu = CPU_ALLOC(1);
    int size = CPU_ALLOC_SIZE(1);
    CPU_ZERO_S(size, cpu);
    CPU_SET_S(thread_id, size, cpu);
    int s = pthread_setaffinity_np(thread, size, cpu);
    if (s != 0) {
        pthread_exit((void *) 1);
    }
    fprintf(stdout, "Thread %d succeded\n", thread_id);
}
