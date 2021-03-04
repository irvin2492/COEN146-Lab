/*
# Name: Irvin Samuel
# Date: Tuesday, 26 Jan 2021
# Title: Lab4 Part1 (TCP/IP Socket programming)
# Description: This program implements socket programming and simulates a client transmitting a file
*/
#include <stdlib.h>
#include <stdio.h>      /* printf, stderr */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>



void* send_message(void* n);

int main(int argc, char* argv[]){
  char* ip_dest = "127.0.0.1";


  int port = atoi(argv[1]);
  char buffer;

  int sockfd = socket(AF_INET, SOCK_STREAM,0);

  struct sockaddr_in address; //stores IP address, port #
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr("127.0.0.1"); // converts from string to bytes
  address.sin_port = htons(port);
  connect(sockfd, (struct sockaddr*)&address, sizeof(address));
  pthread_t t[5];
  int conn_fd[5];

  for(int j = 0; j<5;j++){
    conn_fd[j] = pthread_create(&t[j], NULL, send_message, (void*) sockfd);
  }

  for(int k = 0; k < 5; k++){
    pthread_join(t[k], NULL); //ends the threads before ending the main program
  }
  return close(sockfd);
}

void* send_message(void* n){
  char* thread_src_filename = "src";
  char* thread_ext = ".dat";
  static int x = 0;// made static so the variable can update everytime the program is written
  char num = (x) +'0';
  int sockfd = (int) n;
  size_t len1 = strlen(thread_src_filename) + strlen(&num) + strlen(thread_ext) + 1;

  char* file_src = (char *) malloc(sizeof(char)*len1); //allocates sufficient size for the destination file name

  snprintf(file_src,len1,"%s%c%s", thread_src_filename, num, thread_ext);//concatenates

  FILE* src;
  src = fopen(file_src, "r"); //opens file
  if(src == NULL){
    printf("ERROR opening source file\n");
    exit(0);
  }

  char buffer;

  while(fread(&buffer,1, 1,src)){ // loop to read through the source file byte by byte
    send(sockfd, &buffer,1,0);
  }

  fclose(src);
}
