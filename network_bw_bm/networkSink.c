// Copyright 2016 the project authors as listed in the AUTHORS file.
// All rights reserved. Use of this source code is governed by the
// license that can be found in the LICENSE file.

// Author Panos Patros, 2016

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <strings.h>


int main( int argc, char *argv[] ) {
   int sockfd, newsockfd, portno, clilen;
   char buffer[1024];
   struct sockaddr_in serv_addr, cli_addr;
   int  n;
   
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = 5555;
   if(argc > 1){
	   portno = atoi(argv[1]);
   }
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }
   
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   while(1){
	   printf("Waiting...\n");
	   newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
	   if(fork() == 0){
		   printf("New client!\n");
			
		   if (newsockfd < 0) {
			  perror("ERROR on accept");
			  exit(1);
		   }
		   
		   bzero(buffer,1024);
		   int count = 0;
		   while((n = read( newsockfd,buffer,1024 ))>0){
			   if(count == 0){
				   printf(".");
			   }
			   count++;
			   if(count >= 1024){
				   count = 0;
			   }
		   }
		   
		   close(newsockfd); 
	   }		   
   }
   return 0;
}