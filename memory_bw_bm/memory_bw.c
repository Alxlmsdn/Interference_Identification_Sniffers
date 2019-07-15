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

//#define DEBUG

#define STREAM_ELEMENTS double

void memory_bw_bm(void *);

static uint32_t iterations;
static uint32_t array_size;
static double average_times[4] = {0};
static double num_bytes[4];
pthread_mutex_t console_mutex;

int main(int argc, char* argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Usage: <array size> <number of iterations>\n");
        return 1;
    }
    array_size = atoi(argv[1]);
    iterations = atoi(argv[2]);
    if (pthread_mutex_init(&console_mutex, NULL) != 0) {
        fprintf(stdout, "mutex failed\n");
    }

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
    fflush(stdout);
    free(threads);
    
#ifdef DEBUG
    fprintf(stdout, "....Finished....\n");
#endif
    return 0;
}

void memory_bw_bm(void* tid) {
    uint32_t id = *(uint32_t*) tid;
    //fprintf(stdout, "done %d\n", id);
    STREAM_ELEMENTS times[5][iterations];
    double elapsed_times[5][iterations];
    STREAM_ELEMENTS *a, *b, *c;
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
        times[4][n] = times[0][n];
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
        times[4][n] = cc_get_seconds(0) - times[4][n];
        times[3][n] = cc_get_seconds(0) - times[3][n];
        elapsed_time += times[4][n];
        pthread_mutex_lock(&console_mutex);
        fprintf(stdout, "thread:[%d],rate:[%f](MB/s),totalTime:[%f](s)\n", id, 1e-6*(num_bytes[3]+num_bytes[2]+num_bytes[1]+num_bytes[0])/times[4][n], elapsed_time);
        pthread_mutex_unlock(&console_mutex);
    }

    free(a);
    free(b);
    free(c);
}
