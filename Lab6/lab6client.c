/* * Name: Irvin Samuel
* Date: 9 Feb 2021
* Title: Lab6 - Stop and Wait for an unreliable channel with Loss
* Description: This program is a client who sends a file packet by packet
over to a server using a UDP connection. Upon receiving the packets the server sends an ack for each packet.
Implements a timeout and duplicate acks to register when packets are dropped.
*/

#include "lab6.h"

//getChecksum()
int getChecksum(Packet packet) {
    packet.header.cksum = 0;
    int checksum = 0;
    char *ptr = (char *)&packet;
    char *end = ptr + sizeof(Header) + packet.header.len;
    while (ptr < end) {
        checksum ^= *ptr++;
    }
        return checksum;
}

void printPacket(Packet packet) {
    printf("Packet{ header: { seq_ack: %d, len: %d, cksum: %d }, data: \"",
            packet.header.seq_ack,
            packet.header.len,
            packet.header.cksum);
    fwrite(packet.data, (size_t)packet.header.len, 1, stdout);
    printf("\" }\n");
}

void ClientSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, Packet packet, unsigned retries) {
    while (1) {
      sleep(1);
        //if retries is greater than 3, program moves on. However those bytes are lost
	    if(retries>=3){
		    break;
	    }

        // Print what is being sent
        printf("Created: ");
        printPacket(packet);

        // Simulate lost packet.
        if (rand() % PLOSTMSG == 0) {
            printf("Dropping packet\n");
        } else {
          int bytes_sent = sendto(sockfd, &packet, sizeof(packet.header)+packet.header.len, 0, address, addrlen);
          printf("%d bytes sent \n", bytes_sent);
          }

        struct timeval tv;
        tv.tv_sec = 1; // seconds
        tv.tv_usec = 0;//microseconds
        int rv;
        //setup select parameters
        fd_set readfds;
        fcntl(sockfd,F_SETFL,O_NONBLOCK);
        //start before calling select
        FD_ZERO(&readfds);
        FD_SET(sockfd,&readfds);

        //call select. sockfd+1 is used because select checks upto sockfd-1 descriptors.
        rv=select(sockfd+1, &readfds,NULL,NULL, &tv);
        if(rv==0){ // no data has been received & no sockets are ready
            printf("Timeout\n");
            retries++;
        }
        else if(rv>0){ //if rv!=0, there is something to receive

          // receive an ACK from the server
          Packet recvpacket;
          recvfrom(sockfd, &recvpacket, sizeof(recvpacket), 0,NULL,NULL);

          // print what was received
          printf("Received ACK %d, checksum %d - ", recvpacket.header.seq_ack, recvpacket.header.cksum);

          // validate the ACK
          if (recvpacket.header.cksum != getChecksum(recvpacket)) {
              // bad checksum, resend packet
              printf("Bad checksum, expected %d\n", getChecksum(recvpacket));
          } else if (recvpacket.header.seq_ack != packet.header.seq_ack) {
              // incorrect sequence number, resend packet
              printf("Bad seqnum, expected %d\n", packet.header.seq_ack);
          } else{
              // good ACK, we're done
              printf("Good ACK\n");
              break;
          }
      }
      packet.header.cksum = getChecksum(packet);//if theres a bad sequence number or a bad checksum it will resend the packets

    }
    printf("\n");
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

    int sockfd = socket(AF_INET,SOCK_DGRAM, 0);

    // initialize the server address structure using argv[2] and argv[1]
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(server_ip);
    address.sin_port = htons(port);

    // open file
    FILE *file = fopen(argv[3], "rb");
    if (file == NULL) {
        perror("fopen");
        return 1;
    }

    // send file contents to server
    Packet packet,endPacket;
    int seqnum = 0;
    int bytes_read;

    while((bytes_read = fread(&packet.data, 1,sizeof(packet.data), file))>0){ // reads ten bytes into packet data
      packet.header.seq_ack = seqnum;
      packet.header.len = bytes_read; // must be less than 10
      packet.header.cksum = getChecksum(packet);

      ClientSend(sockfd,(struct sockaddr*) &address, sizeof(address), packet,0);
      seqnum = (seqnum+1) % 2;
     }

    endPacket.header.len = 0;
    endPacket.header.seq_ack = seqnum;
    endPacket.header.cksum = getChecksum(packet);
    ClientSend(sockfd, (struct sockaddr*) &address, sizeof(address), endPacket,0);

    fclose(file);
    close(sockfd);
    return 0;
}
