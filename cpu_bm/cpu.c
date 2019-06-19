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
#include <cctimer.h>
#include <math.h>

uint32_t threadMain();

uint32_t num_threads;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "usage: <number of threads> <prime number to be used> <length of program run time>\n");
        return 1;
    }

    num_threads = atoi(argv[1]);
    uint32_t prime_number = atoi(argv[2]);
    double run_time = atof(argv[3]);

    uint32_t volatile ret_val = threadMain(prime_number, run_time);

    return 0;
}


uint32_t threadMain(uint32_t number, double run_time) {
    uint32_t ret = 1;
    /**Critcal Section **/
    
    double volatile elapsed_time = 0;
    double volatile run_time_s;

    while (elapsed_time < run_time) {
        cctime_t volatile start_time = cc_get_seconds(0);
        for (uint32_t i = 2; i < number; i++) {
            if (number % i == 0) {
                ret = 0;
            }
        }
        cctime_t volatile stop_time = cc_get_seconds(0);
        run_time_s = (double) stop_time - start_time;
        elapsed_time += run_time_s;
        fprintf(stdout, "thread:[%d],runTime:[%f],totalTime:[%f]\n", num_threads, run_time_s * 1.0E3, elapsed_time);
    }
    return ret;
}