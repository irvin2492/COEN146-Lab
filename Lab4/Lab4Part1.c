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

int main(int argc, char* argv[]){
  char* ip_dest = "127.0.0.1";

  FILE* src;
  src = fopen(argv[1], "r"); //opens file specified in the arguments
  if(src == NULL){
    printf("ERROR opening source file\n");
    exit(0);
  }
  int port = atoi(argv[2]);
  char buffer;

  int sockfd = socket(AF_INET, SOCK_STREAM,0);

  struct sockaddr_in address; //stores IP address, port #
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr("127.0.0.1"); // converts from string to bytes
  address.sin_port = htons(port);
  connect(sockfd, (struct sockaddr*)&address, sizeof(address));

  while(fread(&buffer,1, 1,src)){ // loop to read through the source file byte by byte
    send(sockfd, &buffer,1,0);
  }

  fclose(src);
  return close(sockfd);
}
