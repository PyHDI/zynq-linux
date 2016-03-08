#include <stdio.h>
#include <stdlib.h>
#include "axis.h"

int main(int argc, char** argv)
{
  if(argc < 2){
    printf("Usage: ./a.out value\n");
    return 0;
  }

  axis_open();
  
  unsigned int value = atoi(argv[1]);

  axis_write_4b(2, value);
  printf("write: %d\n", value);

  axis_read_4b(2, &value);
  printf("read: %d\n", value);

  axis_close();

  return 0;
}
