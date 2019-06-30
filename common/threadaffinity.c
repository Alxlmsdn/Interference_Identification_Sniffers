////////////////////////////////////////////////////////////////////////////////
// Thread affinity (pinning) interface
// Author: Alex Lumsden
// Date  : 2019

#include "threadaffinity.h"
#include <unistd.h>

#define DEBUG

cpu_thread_t** setThreads(void (*f)(void *), int num_cores_total) {

#ifdef DEBUG
    int num_cores_available = sysconf(_SC_NPROCESSORS_ONLN);
    fprintf(stdout, "number of cores available: %d, from total cores: %d\n", num_cores_available, num_cores_total);
#endif

    cpu_thread_t** threads = malloc(sizeof(cpu_thread_t) * num_cores_total);
    uint32_t thread_id[num_cores_total];
    for (uint32_t i = 0; i < num_cores_total; i++) {
        threads[i] = (cpu_thread_t *) malloc(sizeof(cpu_thread_t));
        thread_id[i] = i;
        threads[i]->id = thread_id[i];
        threads[i]->func = f;
        pthread_t thread;
        threads[i]->thread = &thread;
        cpu_set_t* cpu_set = CPU_ALLOC(1);
        int size = CPU_ALLOC_SIZE(1);
        CPU_ZERO_S(size, cpu_set);
        CPU_SET_S(thread_id[i], size, cpu_set);
        threads[i]->cpu_set = cpu_set;

        pthread_create((threads[i]->thread), NULL, setAffinity, threads[i]);
    }
    return threads;
}


void * setAffinity(void * param) {
    cpu_thread_t * cpu_thread = (cpu_thread_t *) param;
    pthread_t thread = pthread_self();
    int s = pthread_setaffinity_np(thread, sizeof(cpu_thread->cpu_set), cpu_thread->cpu_set);
    if (s != 0) {
        fprintf(stdout, "Thread %d failed\n", cpu_thread->id);
        CPU_FREE(cpu_thread->cpu_set);
        pthread_exit((void*) 1);
    }
    else {
        fprintf(stdout, "Thread %d succeded\n", cpu_thread->id);
        (cpu_thread->func)(&(cpu_thread->id));
    }
    return (void *) 0;
}

void freeCPUSet(cpu_set_t* c_set) {
    CPU_FREE(c_set);
}