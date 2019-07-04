/////////////////////////////////////////////////////////////////
// Author: Alexander Lumsden 2019
//      disk_bw.c is responsible for running the micro-
//      benchmark that will determine the current bandwidth
//      of the storage system of the machine, uses the STREAM metrics 
//      that measures the number of bytes written and bytes
//      read. The benchmark will be multithreaded to determine
//      the bandwidth of each core of the machine.
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

uint32_t runTime;
pthread_mutex_t console_mutex;

void disk_bw_bm(void *);

int main(int argc, char* argv[]) {

    if (argc != 2) {
        fprintf(stdout, "Usage: <runTime>");
        return 1;
    }

    runTime = atof(argv[1]);
    pthread_mutex_init(&console_mutex, NULL);

    int num_cores = sysconf(_SC_NPROCESSORS_CONF);

    cpu_thread_t** threads = setThreads(disk_bw_bm, num_cores);

    for (uint32_t i = 0; i < num_cores; i++) {
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


void disk_bw_bm(void *tid) {
  uint32_t id = *(uint32_t *) tid;
  //fprintf(stdout, "Core ID: %d\n", id);
  char* filename = "thread_" + id + "_file.txt";
  int fd = open(filename, );
  if (fd == -1) {
    fprintf(stdout, "Error opening %s", filename);
    pthread_exit((void *) 1);
  }

  
}
