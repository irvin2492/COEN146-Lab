/* * Name: Irvin Samuel
* Date: 2 Feb 2021
* Title: Lab5 - Stop and Wait for an unreliable channel
* Description: This program is a server which receives packets over a UDP connection
 and responds with an ACK to the client.
*/

//Lab5 Server
#include "lab5.h"
#include <sys/socket.h>
#include<sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>      /* printf, stderr */
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


//calculates correct checksum for a packet
int getChecksum(Packet packet) {
    packet.header.cksum = 0;
    int checksum = 0;
    char *ptr = (char *)&packet;
    char *end = ptr + sizeof(Header) + packet.header.len;
    while (ptr < end) {
        checksum ^= *ptr++; //must xor the contents of the pointer not the ptr itself
    }
    return checksum;
}

// printPacket()
void printPacket(Packet packet) {
    printf("Packet{ header: { seq_ack: %d, len: %d, cksum: %d }, data: \"",
            packet.header.seq_ack,
            packet.header.len,
            packet.header.cksum);
    fwrite(packet.data, (size_t)packet.header.len, 1, stdout);
    printf("\" }\n");
}

//Server sending ACK to the client, len must be zero
void serverSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, int seqnum) {
    Packet packet;
    packet.header.seq_ack = seqnum;
    packet.header.len = 0;
    packet.header.cksum = getChecksum(packet);

    sendto(sockfd, &packet,sizeof(packet), 0, address, addrlen);

    printf("\t Server: sending ACK %d, checksum %d\n", packet.header.seq_ack, packet.header.cksum);
}

//Server receiving packet
Packet serverReceive(int sockfd, struct sockaddr *address, socklen_t *addrlen, int seqnum) {
    Packet packet;
    while (1){
          //recv packets from the client
        recvfrom(sockfd, &packet, sizeof(packet), 0,(struct sockaddr*)address, addrlen);

        // Print received packet
        printPacket(packet);

        if (packet.header.cksum != getChecksum(packet)) {
            printf("Bad checksum, expected checksum was: %d\n", getChecksum(packet));
            serverSend(sockfd, address, *addrlen, !seqnum); //moves to next seqnum since it received a bad packet
        } else if (packet.header.seq_ack != seqnum) {
            printf("Bad seqnum, expected sequence number was:%d\n",seqnum);
            serverSend(sockfd, address, *addrlen, !seqnum);
        } else {
            printf("Good packet\n");
            serverSend(sockfd,address, *addrlen, seqnum); //sends ack back
            break;
        }
    }
    return packet;
}


int main(int argc, char *argv[]) {
    // check arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <port> <outfile>\n", argv[0]);
        exit(1);
    }
    int port = atoi(argv[1]);

    int sockfd = socket(AF_INET,SOCK_DGRAM,0);

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    int addrlen = sizeof(address);
    int bindfd = bind(sockfd,(struct sockaddr*) &address, sizeof(address));

    int file = open(argv[2],O_CREAT | O_WRONLY, 0644);
    if(file == -1){
      printf("ERROR opening source file\n");
      exit(1);
    }

    int seqnum = 0;
    Packet packet;
    do {
        packet = serverReceive(sockfd, (struct sockaddr*) &address, (socklen_t *)&addrlen, seqnum);
        int w = write(file,packet.data,packet.header.len); // writes data from each packet to file, piece by piece
        seqnum = (seqnum+1) % 2; // cycles between 0 & 1

    } while(packet.header.len != 0);

    close(file);
    close(sockfd);
    return 0;
}
