/*
# Name: Irvin Samuel
# Date: Tuesday, 5 Jan 2021
# Title: Lab1 Part 2(Threading)
# Description: This program uses threading to run two parent and child functions

*/

#include <stdio.h>      /* printf, stderr */
#include <sys/types.h>  /* pid_t */
#include <unistd.h>     /* fork */
#include <stdlib.h>     /* atoi */
#include <errno.h>      /* errno */
#include <pthread.h>

// int pthread_create(pthread_t *thread, pthread_attr_t *attr,
//                      void *(*start_routine) (void *arg), void *arg);

void *print_parent(int n);
void *print_child(int n);
/* main function with command-line arguments to pass */
int main(int argc, char *argv[]) {
  pid_t  pid;
  int i, n = atoi(argv[1]); // n microseconds to input from keyboard for delay
    printf("\n Before forking.\n");
    pthread_t t1, t2;

    int c1,c2;

    c1=pthread_create(&t1, NULL,print_parent,n);
    c2=pthread_create(&t1, NULL,print_child,n);

    pthread_join( t1, NULL);
    pthread_join( t2, NULL);
    printf("Thread 1 returns: %d\n",c1);
    printf("Thread 2 returns: %d\n",c2);

  return 0;
}

void *print_parent(int n){
  int i;
  for (i=0;i<100;i++) {
    printf("\t \t \t Parent Process %d \n",i);
    usleep(n);
  }
}
void *print_child(int n){
  int i;
  for (i=0;i<100;i++) {
    printf("Child process %d\n",i);
    usleep(n);
  }
}
