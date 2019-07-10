/////////////////////////////////////////////////////////////////
// Author: Alexander Lumsden 2019
//      disk_bw.c is responsible for running the micro-
//      benchmark that will determine the current bandwidth
//      of the storage system of the machine, uses the STREAM metrics 
//      that measures the number of bytes written and bytes
//      read. The benchmark will be multithreaded to determine
//      the bandwidth of each core of the machine.
//



//#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <cctimer.h>
#include <threadaffinity.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#include <cclfsr.h>

double run_time;
long write_size;
pthread_mutex_t console_mutex;

void disk_bw_bm(void *);

int main(int argc, char* argv[]) {

    if (argc != 3) {
        fprintf(stdout, "Usage: <runTime> <write size>\n");
        return 1;
    }

    run_time = atof(argv[1]);
    write_size = atoi(argv[2]);

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
    fflush(stdout);
    free(threads);
    
#ifdef DEBUG
    fprintf(stdout, "....Finished....\n");
#endif

    return 0;
}


void disk_bw_bm(void *tid) {
    uint32_t id = *(uint32_t *) tid;
    char file_name[20];
    sprintf(file_name, "thread_%d_file.txt", id);
    mode_t mode = S_IRWXU;
    struct stat stats;
    //opens file so program can read,write and program will block until data is written to disk
    int fd = open(file_name, O_RDWR | O_CREAT | O_SYNC, mode);
    if (fd == -1) {
        fprintf(stdout, "Error opening %s\n", file_name); 
        pthread_exit((void *) 1);
    }

    char *write_buffer, *read_buffer;
    long length;
    if (write_size == 0) {
        fstat(fd, &stats);
#ifdef DEBUG
        fprintf(stdout, "file block size: %ld\n", stats.st_blksize); 
#endif
        length = stats.st_blksize;
    }
    else {
        length = write_size;
    }

    write_buffer = (char *) malloc(sizeof(char) * length);
    read_buffer = (char *) malloc(sizeof(char) * length);
    
    cc_lfsr_t cclfsr;
    cc_lfsr_init(&cclfsr, 1, 5);
#ifdef DEBUG
    fprintf(stdout, "Bytes to write: %lu\n", length);
#endif

    double elapsed_time = 0;
    while (elapsed_time < run_time) {
        
        for (uint32_t i = 0; i < length; i++) {
            write_buffer[i] = (cclfsr.value % 10) + '0';
            cclfsr.value = cc_lfsr_next(&cclfsr);
        }

        int written = 0, ret;
        char *ptr = write_buffer;

        double start_time = cc_get_seconds(0);

        while (written < length) {
            ret = write(fd, ptr, length - written);
            if (ret < 0) {
                fprintf(stdout, "Error writing to %s\n", file_name);
                perror("writing to file"); 
                pthread_exit((void *) 1);
            }
            written += ret;
            ptr += ret;
            fsync(fd);
        }

        long buf_read = read(fd, read_buffer, length);
        if (buf_read < 0) {
            perror("read");
            pthread_exit((void *) 1);
        }
        fsync(fd);
        double stop_time = cc_get_seconds(0);
        elapsed_time += (stop_time - start_time);
        pthread_mutex_lock(&console_mutex);
        fprintf(stdout, "Thread:[%d], Bytes:[%lu], runTime(ms):[%f], totalTime:[%f]\n", id, length, 1e3 * (stop_time - start_time), elapsed_time);
        pthread_mutex_unlock(&console_mutex);
    }

#ifndef DEBUG
    int status = remove(file_name);
    if (status != 0) {
        perror("Deleting file");
        pthread_exit((void *) 1);
    }
#endif
    free(write_buffer);
    free(read_buffer);
}
