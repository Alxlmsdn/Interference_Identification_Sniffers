////////////////////////////////////////////////////////////////////////////////
// Thread affinity (pinning) interface
// Author: Alex Lumsden
// Date  : 2019

#define _GNU_SOURCE
#include <limits.h>
#include <sched.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct cputhread {
        pthread_t* thread;
        cpu_set_t* cpu_set;
        uint32_t id;
        void (*func)(void *);
} cpu_thread_t;

cpu_thread_t** setThreads(void (*f)(void *), int);

void * setAffinity();

void freeCPUSet(cpu_set_t*);


