/////////////////////////////////////////////////////////////////
// Author: Alexander Lumsden 2019
//      network_bw is the micro benchmark that is responsible for
//      stressing the outgoing network bandwidth of the system.
//      The bm will use a two programs, one will be responsible
//      for sending a random message to a receiving network sink
//      program hosted on another system (not the target) the
//      throughput in MB/s will be measured. 
//

#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <cctimer.h>
#include <threadaffinity.h>
#include <pthread.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>



int main(int argc, char *argv[]) {

    if (argc < 4) {
        fprintf(stderr, "Usage: <url/IP> <port> <messageSize>\n");
        return -1;
    }

    int port = atoi(argv[2]);
    int msg_length = atoi(argv[3]);
    char *ip = argv[1];
    char *msg_buff = (char *) malloc(sizeof(char) * msg_length);
    for (uint32_t i=0; i < msg_length; i++) {
        msg_buff[i] = (rand() % 10) + '0';
    }

    int socketfd = 0;
    struct sockaddr_in serv_addr;

    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "\n Error : Could not create socket \n");
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(ip);//"127.0.0.1"

    if(connect(socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
        fprintf(stderr, "\n Error : Connect Failed \n");
        return 1;
    }

    int written = 0, ret;
    char *ptr = msg_buff;

    while (written < msg_length) {
        ret = write(socketfd, ptr, msg_length - written);
        if (ret < 0) {
            fprintf(stdout, "Error writing file\n");
            perror("writing to file"); 
            pthread_exit((void *) 1);
        }
        written += ret;
        ptr += ret;
    }

    close(socketfd);
    free(msg_buff);
    return 0;
}