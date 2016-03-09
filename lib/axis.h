#ifndef AXIS_H
#define AXIS_H

#define NUM_AXIS (8)
#define AXIS_SIZE (0x00001000)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <fcntl.h>

int fd_axis [NUM_AXIS];
volatile void* axis_ptr [NUM_AXIS];

void uio_name(char* uio, int i)
{
  char buf [128];
  sprintf(buf, "%d", i);
  strcat(uio, buf);
}

void axis_open()
{
  int i;
  for(i=0; i<NUM_AXIS; i++){
    char uio [1024] = "/dev/uio";
    uio_name(uio, i);
    fd_axis[i] = open(uio, O_RDWR);
    if(fd_axis[i] < 1){
      printf("Invalid UIO device file: '%s'\n", uio);
      exit(1);
    }
    axis_ptr[i] = (volatile void*) mmap(NULL, AXIS_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd_axis[i], 0);
  }
}

void axis_write_4b(unsigned int id, unsigned int offset, unsigned int data)
{
  *(volatile unsigned int*)(axis_ptr[id] + offset) = data;
}

void axis_read_4b(unsigned int id, unsigned int offset, unsigned int* data)
{
  volatile unsigned int r = *(volatile unsigned int*)(axis_ptr[id] + offset);
  *data = r;
}

void axis_close()
{
  int i;
  for(i=0; i<NUM_AXIS; i++){
    munmap((void*) axis_ptr[i], AXIS_SIZE);
    axis_ptr[i] = NULL;
    close(fd_axis[i]);
    fd_axis[i] = -1;
  }
}

#endif
