
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <cctimer.h>
#include <threadaffinity.h>
#include <math.h>
#include <unistd.h>

void icache(void *);
double runTime;
uint32_t size;

int main(int argc, char *argv[]) {
    if(argc != 3) {
        fprintf(stderr, "ERROR: usage <arraySize> <runTime>\n");
        return 1;
    }
    runTime = atof(argv[2]);
    size = atoi(argv[1]);

    int num_cores = sysconf(_SC_NPROCESSORS_CONF);

    cpu_thread_t** threads = setThreads(icache, num_cores);

    for (uint32_t i = 0; i < num_cores; i++) {
        pthread_join(*(threads[i]->thread), NULL);
    }
    for(uint32_t i = 0; i < num_cores; i++) {
        freeCPUSet(threads[i]->cpu_set);
        free(threads[i]);
    }
    fflush(stdout);
    free(threads);

    return 0;
}

void icache(void *tid) {
    uint32_t id = *(uint32_t *) tid;
    extern func1();
    extern func2();
    extern func3();
    extern func4();
    extern func5();
    extern func6();
    extern func7();
    extern func8();
    extern func9();
    extern func10();
    extern func11();
    extern func12();
    //int size = 1000000;
    int *calls = malloc(sizeof(int) * size);
    for (int i = 0; i < size; i++) {
        calls[i] = (rand() % 12) + 1;
    }
    double total_run_time = 0;
    while(total_run_time < runTime) {
        double start_time = cc_get_seconds(0);
        for (int i = 0; i < size; i++) {
            switch (calls[i]) {
                case 1:
                    func1();
                    break;
                case 2:
                    func2();
                    break;
                case 3:
                    func3();
                    break;
                case 4:
                    func4();
                    break;
                case 5:
                    func5();
                    break;
                case 6:
                    func6();
                    break;
                case 7:
                    func7();
                    break;
                case 8:
                    func8();
                case 9:
                    func9();
                    break;
                case 10:
                    func10();
                    break;
                case 11:
                    func11();
                    break;
                case 12:
                    func12();
                    break;
                default:
                    break;
            }
        }
        double stop_time = cc_get_seconds(0);
        total_run_time += stop_time - start_time;
        fprintf(stdout, "thread:[%d],runTime:[%f],totalTime:[%f]\n", id, stop_time - start_time, total_run_time);
    }
    free(calls); 
}

/*void func1() {
    for(int i = 0; i < 4095; i++) {
        __asm__ volatile ("nop\n\t");
    }
}*/