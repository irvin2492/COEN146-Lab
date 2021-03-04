/*
# Name: Irvin Samuel
# Date: Tuesday, 18 Jan 2021
# Title: Lab3 Part 3(Network Commands on the Web)
# Description: This program simulates the time needed to load an HTMl page with objects Given
diffferent circumstances: Non-Persistent requests, persistent requests, and parallel requests.

*/

#include <stdio.h>      /* printf, stderr */
#include <errno.h>      /* errno */

void a(int t1, int t2, int t3, int t4);
void b(int t1, int t2, int t3, int t4);
void c1(int t1, int t2, int t3, int t4, int gSize);
void c2(int t1, int t2, int t3, int t4, int gSize);


int main(int argc, char *argv[]) {
int RTT0 = 3; // round trip time to local DNS cache
int RTT1 = 20;
int RTT2 = 26;
int RTTHTTP = 47;

a(RTT0, RTT1, RTT2, RTTHTTP);
b(RTT0, RTT1, RTT2, RTTHTTP);
c1(RTT0, RTT1, RTT2, RTTHTTP,3);
c2(RTT0, RTT1, RTT2, RTTHTTP,3);


return 0;
}
void a(int t1, int t2, int t3, int t4){
  int timeElapsed = t1 + t2 + t3 + (2*t4);
  printf("Time elapsed to connect to load HTML page a: %dms\n",timeElapsed);
}

void b(int t1, int t2, int t3, int t4){ //no parallel non-persistent
  int timeElapsed = t1 + t2 + t3 + (2*t4); //time to check local cache then connect to DNS 1 & 2 and retrieve HTML object
  for(int i = 0; i<6; i++){
    timeElapsed += (2*t4); //RTT for TCP connection & HTTP request for each object
  }
  printf("Time elapsed to connect to load HTML page b: %dms\n",timeElapsed);
}

void c1(int t1, int t2, int t3, int t4, int gSize){ //non-persistent
  if(gSize<= 0 || gSize > 6){
    printf("invalid group size.\n");
    return;
  }
  int timeElapsed = t1 + t2 + t3 + (2*t4); //time to check local cache, connect to DNS 1 & 2,establish TCP conn, & retrieve HTML object

  int objs = 6;
  int groups = 1;

  while(objs > gSize){
    objs -= gSize;
    groups++;
  }

  for(int i = 0; i < groups; i++){
    timeElapsed += (2 * t4); //time to reach local DNS cache and RTT for HTTP request for each object
  }

  printf("Time elapsed to connect to load HTML page c1: %dms\n",timeElapsed);
}

void c2(int t1, int t2, int t3, int t4, int gSize){ // persistent
  if(gSize<= 0 || gSize > 6){
    printf("Invalid group size.\n");
    return;
  }
  int timeElapsed = t1 + t2 + t3 + (2*t4); // time to connect to DNS retrieve HTML object

  int objs = 6;
  int groups = 1;

  while(objs > gSize){
    objs -= gSize;
    groups++;
  }

  for(int i = 0; i < groups; i++){
    timeElapsed += t4; //time to reach local DNS cache and RTT for HTTP request for each object
  }

  printf("Time elapsed to connect to load HTML page c2: %dms\n",timeElapsed);

}
