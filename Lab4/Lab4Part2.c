/*
# Name: Irvin Samuel
# Date: Tuesday, 26 Jan 2021
# Title: Lab4 Part 2  (TCP/IP Socket programming)
# Description: This program implements socket programming and simulates a server listening for and receiving a message
*/
#include <stdlib.h>
#include <stdio.h>      /* printf, stderr */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[]){
  int port = atoi(argv[1]);
  FILE* dest = fopen("dest1.txt", "w");

  if(dest == NULL){
    printf("ERROR opening destination file\n");
    exit(0);
  }

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in address; //stores IP address, port #
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  int bind_val =bind(sockfd, (struct sockaddr*) &address, sizeof(address));
  int listen_val = listen(sockfd,1); // checks for TCP connection, if so, returns TCP ack
  int address_size = sizeof(address);
  int connectionfd = accept(sockfd,(struct sockaddr*) &address, (socklen_t*) &address_size);

  char message;

  while(recv(connectionfd, &message, 1,0) > 0){ // loops until there are no more bytes to receive
    fwrite(&message,1,1,dest);
  }
  fclose(dest);
  close(connectionfd);
  return close(sockfd);

}
