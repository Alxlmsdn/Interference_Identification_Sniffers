////////////////////////////////////////////////////////////////////////////////
// Thread affinity (pinning) interface
// Author: Alex Lumsden
// Date  : 2019

#include "threadaffinity.h"
#include <unistd.h>

//#define DEBUG
//function that sets a thread for each core passed as argument, and then executes the passed fucntion 
cpu_thread_t** setThreads(void (*f)(void *), int num_cores_total) {

#ifdef DEBUG
    int num_cores_available = sysconf(_SC_NPROCESSORS_ONLN);
    fprintf(stdout, "number of cores available: %d, from total cores: %d\n", num_cores_available, num_cores_total);
#endif
    //create an array of CPU_thread objects 
    cpu_thread_t** threads = malloc(sizeof(cpu_thread_t) * num_cores_total);
    uint32_t thread_id[num_cores_total];
    //create the thread for each core and execute it 
    for (uint32_t i = 0; i < num_cores_total; i++) {
        threads[i] = (cpu_thread_t *) malloc(sizeof(cpu_thread_t));
        //set thread id
        thread_id[i] = i;
        threads[i]->id = thread_id[i];
        threads[i]->func = f;
        pthread_t thread;
        //set the thread field of the CPU_thread 
        threads[i]->thread = &thread;
        //create the cpu set that the will be used for thread pinning
        cpu_set_t* cpu_set = CPU_ALLOC(1);
        int size = CPU_ALLOC_SIZE(1);
        CPU_ZERO_S(size, cpu_set);
        CPU_SET_S(thread_id[i], size, cpu_set);
        threads[i]->cpu_set = cpu_set;
        //create the thread by calling the setAffinity function 
        pthread_create((threads[i]->thread), NULL, setAffinity, threads[i]);
    }
    return threads;
}

//function that will set the affinity of the calling thread, so it only executes on the core specified by the thread id
void * setAffinity(void * param) {
    cpu_thread_t * cpu_thread = (cpu_thread_t *) param;
    pthread_t thread = pthread_self();
    int s = pthread_setaffinity_np(thread, sizeof(cpu_thread->cpu_set), cpu_thread->cpu_set);
    if (s != 0) {
#ifdef DEBUG
        fprintf(stdout, "Thread %d failed\n", cpu_thread->id);
#endif
        //if the thread was unable to be run on the cpu exit the thread 
        CPU_FREE(cpu_thread->cpu_set);
        pthread_exit((void*) 1);
    }
    else {
#ifdef DEBUG
        fprintf(stdout, "Thread %d succeded\n", cpu_thread->id);
#endif
        //otherwise the affinity is set and call the given function
        (cpu_thread->func)(&(cpu_thread->id));
    }
    return (void *) 0;
}

//frees the allocated memory for the cpu_set given 
void freeCPUSet(cpu_set_t* c_set) {
    CPU_FREE(c_set);
}