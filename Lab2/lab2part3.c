/*
# Name: Irvin Samuel
# Date: Tuesday, 12 Jan 2021
# Title: Lab2 Part 3(Threading)
# Description: This program uses threading to run 10 system calls that read from 10 files
and create ten new destination files to copy the data into.

*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void *copy_file(void* num);

int main(int argc, char * argv[]){

  pthread_t t[10];
  int p[10];
  for(int j = 0; j < 10; j++){
    p[j] = pthread_create(&t[j], NULL, copy_file, (void *)j); //creates ten threads for each copy file
  }

  for(int k = 0; k < 10; k++){
    pthread_join(t[k], NULL); //ends the threads before ending the main program
  }
  return 0;
}
void *copy_file(void* n){
  char* thread_src_filename = "thread_src";
  char* thread_dest_filename = "thread_dest";
  char* thread_ext = ".dat";
  char num = ((int) n) +'0';
  size_t len1 = strlen(thread_src_filename) + strlen(&num) + strlen(thread_ext) + 1;
  size_t len2 = strlen(thread_dest_filename) + strlen(&num) + strlen(thread_ext) + 1;

  char* file_src = (char *) malloc(sizeof(char)*len1); //allocates sufficient size for the source file name
  char* file_dest = (char *) malloc(sizeof(char)*len2); //allocates sufficient size for the destination file name

  snprintf(file_src,len1,"%s%c%s", thread_src_filename, num, thread_ext); //concatenates file name with extension
  snprintf(file_dest,len2,"%s%c%s", thread_dest_filename, num, thread_ext);
  printf("moving from %s to %s\n",file_src,file_dest);

  int source = open(file_src,O_RDONLY);
  int destination = open(file_dest,O_CREAT | O_WRONLY| O_TRUNC,0644);

  if(source == -1){
    printf("ERROR opening source file\n");
    exit(0);
  }
  if(destination == -1){
    printf("ERROR opening destination file\n");
    exit(0);
  }
  char buff[10]; //buffer array to hold all 100000 random bytes from file

  while(read(src_read,buff, 1) > 0){ //reads each individual byte until the end of the file
    write(dest_write,buff,1);
  }

  close(source);
  close(destination);
}
