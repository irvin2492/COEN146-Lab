/*
# Name: Irvin Samuel
# Date: Tuesday, 5 Jan 2021
# Title: Lab1 Part 3(Circuit switching vs. Packet switching)
# Description: This program compares the effectiveness of the Circuit switching vs Packet switching
*/

#include <stdio.h>      /* printf, stderr */
#include <math.h>

long fact(int n){
  long arr[n+1];
  if(n<=1){
    return 1;
  }
  arr[0]=arr[1]=1;
  for(int i =2; i <= n; i++){
    arr[i] = arr[i-1]*i;
  }
  return(arr[n]);
}
int main(int argc, char *argv[]){
  int linkBandwidth = 200; //variable values given by lab instructions
  int userBandwidth = 20;
  int nCSusers = linkBandwidth / userBandwidth;
  int nPSusers = 19;
  double tPSuser = 0.10;
  double pPSusersBusy = tPSuser;
  double pPSusersNotBusy = 1 - pPSusersBusy;
  double pallusersNotBusy = pow((1 - tPSuser), nPSusers-1);
  printf("\nProbability that all of the other specific other users are not busy : %f\n",pallusersNotBusy);

  double pOneUserTransmitting = pPSusersBusy * pow(pPSusersNotBusy, nPSusers-1);
  printf("\nProbability that one specific user is transmitting and the remaining users are not transmitting  : %f\n",pOneUserTransmitting);

  double pOneUserBusyTransmitting = nPSusers * pOneUserTransmitting;
  printf("\nProbability that exactly one (anyone) of the nPSusersusers is busy: %f\n",pOneUserBusyTransmitting);
  //f
  double pTenSpecificUsersTransmittting = (double) pow(pPSusersBusy, 10.0) *  pow(pPSusersNotBusy, (double) nPSusers-10);
  printf("\nProbability that 10 specific users of nPSusers are transmitting and the others are idle  : %g\n",pTenSpecificUsersTransmittting);
  //g
  double nPSusersChooseTen = ((double)fact(nPSusers) / ((double)(fact(10) * ((double)fact(nPSusers-10)))));
  double pTenAnyUsersTransmitting = nPSusersChooseTen * pow(pPSusersBusy,10.0) * pow(pPSusersNotBusy, (double) nPSusers-10);
  printf("\nThe probability that any 10 users of nPSusers are transmitting and the others are idle : %g\n",pTenAnyUsersTransmitting);

  //h
  double tenOrMoreUsersTransmitting = 0.0;
  for(int i = 11; i <= nPSusers; i++){
    tenOrMoreUsersTransmitting +=  ((double) fact(nPSusers) / (((double) fact(i) * ((double) fact(nPSusers-i)))))
    * (pow(pPSusersBusy, (double) i) *  pow(pPSusersNotBusy, (double) (nPSusers - i)));
  }
  printf("\nThe probability that more than 10 users of nPSusers are transmitting and the others are idle: %g\n",tenOrMoreUsersTransmitting);

}
