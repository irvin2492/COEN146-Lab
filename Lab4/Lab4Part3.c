/*
# Name: Irvin Samuel
# Date: Tuesday, 26 Jan 2021
# Title: Lab4 Part 3  (TCP/IP Socket programming)
# Description: This program implements socket programming and simulates a multi-thread client
*/
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>      /* printf, stderr */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void* connect_client(void* n);

int main(int argc, char* argv[]){
  int port = atoi(argv[1]);

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in address; //stores IP address, port #
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  int bind_val = bind(sockfd, (struct sockaddr*) &address, sizeof(address)); // binds socket to a port and address
  int listen_val = listen(sockfd,5); // checks for TCP connection, if so, returns TCP ack
  int address_size = sizeof(address);
  pthread_t t[5];
  int conn_fd[5];

  int j = 0;

  while(j < 5){
    printf("Awaiting source file %d...\n", j);
    int connectionfd = accept(sockfd,(struct sockaddr*) &address, (socklen_t*) &address_size);
    if(connectionfd > 0){
      conn_fd[j] = pthread_create(&t[j], NULL, connect_client, (void*) connectionfd); // launches thread for every one connection it receives
      j++;
    }
  }

  for(int k = 0; k < 5; k++){
    pthread_join(t[k], NULL); //ends the threads before ending the main program
  }
  close(sockfd);

}

void* connect_client(void* n){
  char* thread_dest_filename = "thread_dest";
  char* thread_ext = ".dat";
  static int x = 0;// made static so the variable can update everytime the program is written
  char num = (x) +'0';
  int file_descriptor = (int) n;
  size_t len1 = strlen(thread_dest_filename) + strlen(&num) + strlen(thread_ext) + 1;

  char* file_dest = (char *) malloc(sizeof(char)*len1); //allocates sufficient size for the destination file name

  snprintf(file_dest,len1,"%s%c%s", thread_dest_filename, num, thread_ext);

  char message;
  FILE* dest = fopen(file_dest, "w");

  if(dest == NULL){
    printf("ERROR opening destination file\n");
    exit(0);
  }

  while(recv(file_descriptor, &message, 1,0) > 0){ // loops until there are no more bytes to receive from that connection
    fwrite(&message,1,1,dest);
  }
  printf("File %d successfully received.\n", x++);

  fclose(dest);
  close(file_descriptor);

}
