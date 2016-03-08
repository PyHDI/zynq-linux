#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#include "cma.h"

int main(int argc, char** argv)
{
  if(argc < 2){
    printf("# Usage: ./a.out size [runmode]\n");
    return -1;
  }

  unsigned int size = atoi(argv[1]);
  int runmode = 0;
  if(argc > 2){
    runmode = atoi(argv[2]);
  }
  printf("# RUN MODE: %d\n", runmode);

  //cma_open(CMA_SYNC_ON_WC);
  unsigned int sync_mode = 0;
  if(runmode > 0) sync_mode = runmode - 1;
  cma_open(sync_mode);

  unsigned int i;
  
  volatile int* a;
  volatile int* b;

  if(runmode == 0){
    a = (volatile int*) malloc(sizeof(int)*size);
    b = (volatile int*) malloc(sizeof(int)*size);
  }else{
    a = (volatile int*) cma_malloc(sizeof(int)*size);
    b = (volatile int*) cma_malloc(sizeof(int)*size);
  }
  
  for(i=0; i<size; i++){
    a[i] = i + 1024;
  }

  if(runmode > 0){
    printf("CMA\n");
    cma_cache_clean((char*)a, size*sizeof(int));
  }

  struct timeval s, e;

  gettimeofday(&s, NULL);

  for(i=0; i<size; i++){
    b[i] = a[i];
  }
  //memcpy((void*)b, (void*)a, size*sizeof(int));

  gettimeofday(&e, NULL);

  if(runmode > 0){
    cma_cache_clean((char*)b, size*sizeof(int));
  }

  printf("# end\n");
  int ok = 1;
  
  for(i=0; i<size; i++){
    if(a[i] != b[i]){
      ok = 0;
      printf("NG: a: %d, b: %d\n", a[i], b[i]);
    }
  }

  if(ok){
    printf("OK\n");
  }
  else{
    printf("NG\n");
  }

  double exec_time = (e.tv_sec - s.tv_sec) + (e.tv_usec - s.tv_usec) * 1.0E-6;
  printf("exectuion time=%lf\n", exec_time);
  
  cma_close();

  return 0;
}
  
