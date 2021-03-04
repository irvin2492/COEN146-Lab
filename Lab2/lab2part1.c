/*
# Name: Irvin Samuel
# Date: Tuesday, 12 Jan 2021
# Title: Lab2 Part 1 (Functions)
# Description: This program uses functions to read 100k bytes from a file
and write that same data into a new destination file.

*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char * argv[]){
  FILE *src;
  FILE *dest;
  src = fopen(argv[1],"r");
  dest = fopen(argv[2],"w");

  if(src == NULL){
    printf("ERROR opening source file\n");
    exit(0);
  }
  if(dest == NULL){
    printf("ERROR opening destination file\n");
    exit(0);
  }
  char buff[100000]; //bufffer array to hold all 100000 random bytes from file
  char c;

  clock_t start,end;
  double time_spent = 0.0;
  start = clock();
  while(fread(&buff,1,1, src)){ //reads each individual byte until the end of the file
    fwrite(&buff,1,1,dest);
  }

  fclose(src);
  fclose(dest);

  end = clock();
  time_spent = ((double) (end-start)) / CLOCKS_PER_SEC;
  printf("Time spent: %f seconds.\n",time_spent);

  return 0;
}
