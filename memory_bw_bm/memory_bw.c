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


# include <stdio.h>
# include <unistd.h>
# include <math.h>
# include <float.h>
# include <limits.h>

#define STREAM_ELEMENTS double

static STREAM_ELEMENTS a,b,c;
static double average_times[4] = {0};
static double num_bytes[4];

int main() {
    
}
