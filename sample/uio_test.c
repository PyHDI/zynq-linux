#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
  if(argc < 2){
    printf("Usage: ./a.out value\n");
    return 0;
  }

  int fd = open("/dev/uio2", O_RDWR);
  if(fd < 0){
    printf("cannot open /dev/uio2\n");
    return -1;
  }

  volatile int* gpio = (volatile int*) mmap(NULL, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

  unsigned int value = atoi(argv[1]);

  *gpio = value;
  printf("write: %d\n", value);

  value = *(gpio + 2);
  printf("read: %d\n", value);

  return 0;
}
