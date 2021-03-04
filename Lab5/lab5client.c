/* * Name: Irvin Samuel
* Date: 2 Feb 2021
* Title: Lab5 - Stop and Wait for an unreliable channel
* Description: This program is a client who sends a file packet by packet
over to a server using a UDP connection. Upon receiving the packets the server sends an ack for each packet.
*/

//Lab5 Client
#include "lab5.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>      /* printf, stderr */
#include <fcntl.h>
#include <string.h>
#include <unistd.h>



//getChecksum()
int getChecksum(Packet packet) {
    packet.header.cksum = 0;

    int checksum = 0;
    char *ptr = (char *)&packet;
    char *end = ptr + sizeof(Header) + packet.header.len;

    while (ptr < end) {
        checksum ^= *ptr++; //must xor the contents of the pointer
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

//clientSend()
void clientSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, Packet packet) {
    while (1) {

        printf("Client: sending packet\n");

        printPacket(packet);

        int bytes_sent = sendto(sockfd, &packet, sizeof(packet.header)+packet.header.len, 0, address, addrlen);
        printf("%d bytes sent \n", bytes_sent);

        // receive an ACK from the server
        Packet recvpacket;
        recvfrom(sockfd, &recvpacket, sizeof(recvpacket), 0,NULL,NULL);

        printf("Received ACK %d, checksum %d - ", recvpacket.header.seq_ack, recvpacket.header.cksum);

        //print received packet
        printPacket(packet);

        // validate the ACK
        if (recvpacket.header.cksum != getChecksum(recvpacket)) {
            // bad checksum, resend packet
            printf("Bad checksum, expected checksum was: %d\n", getChecksum(recvpacket));
        } else if (recvpacket.header.seq_ack != packet.header.seq_ack) {
            // incorrect sequence number, resend packet
            printf("Bad seqnum, expected sequence number was: %d\n",packet.header.seq_ack);
        } else {
            // good ACK, we're done
            printf("Good ACK\n");
            break;
        }
        packet.header.cksum = getChecksum(packet);//if theres a bad sequence number or a bad checksum it will resend the packets

    }
}

int main(int argc, char *argv[]) {
    // check arguments
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <ip> <port> <infile>\n", argv[0]);
        return 1;
    }
    char* server_ip = argv[1];
    int port = atoi(argv[2]);

    // seed the RNG
    srand((unsigned)time(NULL));

    // create and configure the socket [name the socket sockfd]
    int sockfd = socket(AF_INET,SOCK_DGRAM, 0);

    // initialize the server address structure using argv[2] and argv[1]
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(server_ip);
    address.sin_port = htons(port);

    FILE* file = fopen(argv[3], "r");
    if(file == NULL){
      printf("ERROR opening source file\n");
      exit(1);
    }
    int seqnum = 0;
    Packet packet,endPacket;

    int bytes_read;

    while((bytes_read = fread(&packet.data, 1,sizeof(packet.data), file))>0){ // reads ten bytes into packet data
      packet.header.seq_ack = seqnum;
      packet.header.len = bytes_read; // must be less than 10
      if(rand()%5 == 0){
        packet.header.cksum = 0; // if its zero it will cause a bad checksum error
      }else{
        packet.header.cksum = getChecksum(packet);
     }

      clientSend(sockfd, (struct sockaddr*) &address, sizeof(address), packet);
      seqnum = (seqnum+1) % 2;
    }

    endPacket.header.len =0;
    endPacket.header.seq_ack = seqnum;
    endPacket.header.cksum = getChecksum(packet);
    clientSend(sockfd, (struct sockaddr*) &address, sizeof(address), endPacket);

    fclose(file);
    close(sockfd);
    return 0;
}
