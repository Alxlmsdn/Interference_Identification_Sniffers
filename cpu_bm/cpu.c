/////////////////////////////////////////////////////////////////
// Author: Alexander Lumsden 2019
//      cpu.c holds a bench mark that will execute a critical 
//      section that determines if a large number is prime
//      this is repeated, each repetition is timed and recorded 
//      to monitor the state and contention of the cpu over time
//


#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <cctimer.h>
#include <threadaffinity.h>
#include <math.h>
#include <unistd.h>

uint32_t threadMain();
void testing();
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
uint32_t prime_number;
double run_time;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "usage: <number of threads> <prime number to be used> <length of program run time>\n");
        return 1;
    }

    //uint32_t num_threads = atoi(argv[1]);
    int num_cores = sysconf(_SC_NPROCESSORS_CONF);
    prime_number = atoi(argv[2]);
    run_time = atof(argv[3]);

    cpu_thread_t** threads = setThreads(testing, num_cores);
    for(uint32_t i = 0 ; i < num_cores ; i++) {
        pthread_join(*(threads[i]->thread), NULL);
    }
    for(uint32_t i = 0 ; i < num_cores ; i++) {
        freeCPUSet(threads[i]->cpu_set);
        free(threads[i]);
    } 
    free(threads);

    return 0;
}

void testing(void* id) {
    uint32_t ret_val = threadMain(id);
    //return NULL;
}

uint32_t threadMain(void* id) {
    uint32_t* thread_id = (uint32_t *) id;
    /**Critcal Section **/
    uint32_t ret = 0;
    double volatile elapsed_time = 0;
    double volatile run_time_s;

    while (elapsed_time < run_time) {
        cctime_t volatile start_time = cc_get_seconds(0);
        for (uint32_t i = 2; i < prime_number; i++) {
            if (prime_number % i == 0) {
                ret = 1;
            }
        }
        cctime_t volatile stop_time = cc_get_seconds(0);
        run_time_s = (double) stop_time - start_time;
        elapsed_time += run_time_s;
        pthread_mutex_lock(&mutex1);
        fprintf(stdout, "thread:[%d],runTime:[%f],totalTime:[%f]\n", *thread_id, run_time_s * 1.0E3, elapsed_time);
        pthread_mutex_unlock(&mutex1);
    }
    return ret;
}
