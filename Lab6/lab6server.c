/* * Name: Irvin Samuel
* Date: 9 Feb 2021
* Title: Lab6 - Stop and Wait for an unreliable channel with Loss
* Description: This program is a server which receives packets over a UDP connection
 and responds with an ACK to the client.
 Implements a random chance of dropping the Ack
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

//print packet
void printPacket(Packet packet) {
    printf("Packet{ header: { seq_ack: %d, len: %d, cksum: %d }, data: \"",
            packet.header.seq_ack,
            packet.header.len,
            packet.header.cksum);
    fwrite(packet.data, (size_t)packet.header.len, 1, stdout);
    printf("\" }\n");
}

//serverSend()
void serverSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, int seqnum) {
    // Simulating a chance that ACK gets lost
    if (rand() % PLOSTMSG == 0) {
        printf("Dropping ACK\n");
    }
    else{
        Packet packet;
        packet.header.seq_ack = seqnum;
        packet.header.len = 0;
        packet.header.cksum = getChecksum(packet);

        sendto(sockfd, &packet,sizeof(packet), 0, address, addrlen);

        printf("Sent ACK %d, checksum %d\n", packet.header.seq_ack, packet.header.cksum);
    }
}

Packet serverReceive(int sockfd, struct sockaddr *address, socklen_t *addrlen, int seqnum)
{
    Packet packet;
    while (1) {
        //Receive a packet from the client
        recvfrom(sockfd, &packet, sizeof(packet), 0,(struct sockaddr*)address, addrlen);

        // print what was received
        printf("Received: ");
        printPacket(packet);

        //verify the checksum and the sequence number
        if (packet.header.cksum != getChecksum(packet)) {
            printf("Bad checksum, expected %d\n", getChecksum(packet));
            serverSend(sockfd, address, *addrlen, !seqnum); //moves to next seqnum since it received a bad packet
        } else if (packet.header.seq_ack != seqnum) {
            printf("Bad seqnum, expected %d\n", seqnum);
            serverSend(sockfd, address, *addrlen, !seqnum);
        } else {
            printf("Good packet\n");
            serverSend(sockfd,address, *addrlen, seqnum); //sends ack back
            break;
        }
    }
    printf("\n");
    return packet;
}

int main(int argc, char *argv[]) {
    // check arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <port> <outfile>\n", argv[0]);
        exit(1);
    }
    int port = atoi(argv[1]);
    // seed the RNG
    srand((unsigned)time(NULL));

    int sockfd = socket(AF_INET,SOCK_DGRAM,0);

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    int addrlen = sizeof(address);
    int bindfd = bind(sockfd,(struct sockaddr*) &address, sizeof(address));

    // open file using argv[2]
    int file=open(argv[2],O_CREAT | O_RDWR,0666);
    if(file<0){
	    perror("File failed to open\n");
	    exit(1);
    }
    // get file contents from client and save it to the file
    int seqnum = 0;
    Packet packet;
    do {
      packet = serverReceive(sockfd, (struct sockaddr*) &address, (socklen_t *)&addrlen, seqnum);
      int w = write(file,packet.data,packet.header.len); // writes data from each packet to file, piece by piece
      seqnum = (seqnum+1) % 2; // cycles between 0 & 1

    } while (packet.header.len != 0);

    close(file);
    close(sockfd);

    return 0;
}
