/*
# Name: Irvin Samuel
# Date: Tuesday, 12 Jan 2021
# Title: Lab2 Part 2(System Calls)
# Description: This program uses system calls to read 100k bytes from a file
and copy the data into a new destination file.

*/
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char * argv[]){
  int src = open(argv[1],O_RDONLY);
  int dest = open(argv[2],O_WRONLY | O_CREAT | O_TRUNC);
  char buff[10]; //bufffer array to hold all 100000 random bytes from file

  clock_t start,end;
  double time_spent = 0.0;
  start = clock();

  if(src == -1){
    printf("ERROR opening source file\n");
    exit(0);
  }
  if(dest == -1){
    printf("ERROR opening destination file\n");
    exit(0);
  }

  while(read(src,buff, 1) > 0){ //reads each individual byte until the end of the file
    write(dest,buff,1);
  }
  close(src);
  close(dest);

  end = clock();
  time_spent = ((double) (end-start)) / CLOCKS_PER_SEC;
  printf("Time spent: %f seconds.\n",time_spent);

  return 0;
}
