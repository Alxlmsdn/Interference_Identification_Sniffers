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
#include <threadaffinity.h>
#include <pthread.h>

#define DEBUG

#define STREAM_ELEMENTS double

void memory_bw_bm(void *);

static STREAM_ELEMENTS *a, *b, *c;
static uint32_t iterations;
static uint32_t array_size;
static double average_times[4] = {0};
static double num_bytes[4];
pthread_mutex_t console_mutex;

int main(int argc, char* argv[]) {

    if (argc != 4) {
        fprintf(stderr, "Usage: <number of threads> <array size> <number of iterations>\n");
        return 1;
    }
    //uint32_t num_threads = atoi(argv[1]);
    array_size = atoi(argv[2]);
    iterations = atoi(argv[3]);
    if (pthread_mutex_init(&console_mutex, NULL) != 0) {
        fprintf(stdout, "mutex failed\n");
    }

    /* a = (STREAM_ELEMENTS *) malloc(sizeof(STREAM_ELEMENTS) * array_size);
    b = (STREAM_ELEMENTS *) malloc(sizeof(STREAM_ELEMENTS) * array_size);
    c = (STREAM_ELEMENTS *) malloc(sizeof(STREAM_ELEMENTS) * array_size);
    */
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
    for(uint32_t i = 0; i < num_cores; i++) {
        freeCPUSet(threads[i]->cpu_set);
        free(threads[i]);
    }
    free(threads);
    
#ifdef DEBUG
    fprintf(stdout, "....Finished....\n");
#endif
    return 0;
}

void memory_bw_bm(void* tid) {
    uint32_t id = *(uint32_t*) tid;
    //fprintf(stdout, "done %d\n", id);
    STREAM_ELEMENTS times[4][iterations];
    double elapsed_times[4][iterations];
    a = (STREAM_ELEMENTS *) malloc(sizeof(STREAM_ELEMENTS) * array_size);
    b = (STREAM_ELEMENTS *) malloc(sizeof(STREAM_ELEMENTS) * array_size);
    c = (STREAM_ELEMENTS *) malloc(sizeof(STREAM_ELEMENTS) * array_size);
    for (uint32_t i = 0; i < array_size; i++) {
        a[i] = 2.0;
        b[i] = 2.0;
        c[i] = 0.0;
    }
    double elapsed_time = 0;
    double scalar = 3.0;
    for (uint32_t n = 0; n < iterations; n++) {
        times[0][n] = cc_get_seconds(0);
        for (uint32_t i = 0; i < array_size; i++) {
            c[i] = a[i];
        }
        times[0][n] = cc_get_seconds(0) - times[0][n];


        times[1][n] = cc_get_seconds(0);
        for (uint32_t i = 0; i < array_size; i++) {
            b[i] = scalar*c[i];
        }
        times[1][n] = cc_get_seconds(0) - times[1][n];


        times[2][n] = cc_get_seconds(0);
        for (uint32_t i = 0; i < array_size; i++) {
            c[i] = a[i]+b[i];
        }
        times[2][n] = cc_get_seconds(0) - times[2][n];


        times[3][n] = cc_get_seconds(0);
        for (uint32_t i = 0; i < array_size; i++) {
            a[i] = b[i]+scalar*c[i];
        }
        times[3][n] = cc_get_seconds(0) - times[3][n];
        elapsed_time += times[3][n];
        fprintf(stdout, "thread:[%d],rate:[%f](bytes/s),totalTime:[%f](s)\n", id, num_bytes[3]/times[3][n], elapsed_time);
    }
}
