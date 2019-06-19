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

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: <prime number to be used> <length of program run time>");
        return 1;
    }

    uint32_t prime_number = atoi(argv[1]);
    double run_time = atof(argv[2]);

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
        fprintf(stdout, "thread:[0],runTime:[%f],totalTime:[%f]\n", run_time_s, elapsed_time);
    }
    return ret;
}