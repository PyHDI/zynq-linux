#include <stdio.h>
#include <stdlib.h>
#include "axis.h"

int main(int argc, char** argv)
{
  if(argc < 2){
    printf("./a.out value");
    return 0;
  }
  
  unsigned int value = atoi(argv[1]);

  axis_open();

  axis_write_4b(2, value);

  axis_read_4b(2, &value);
  printf("%d\n", value);

  axis_close();

  return 0;
}
