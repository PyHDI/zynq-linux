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
volatile int* axis_ptr [NUM_AXIS];

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
    uio_name(uio, i+1);
    /* "/dev/uio0" is reserved for other purpose */
    fd_axis[i] = open(uio, O_RDWR);
    if(fd_axis[i] < 1){
      printf("Invalid UIO device file: '%s'\n", uio);
      exit(1);
    }
    axis_ptr[i] = (volatile int*) mmap(NULL, AXIS_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd_axis[i], 0);
  }
}

void axis_write_4b(unsigned int id, unsigned int data)
{
  *axis_ptr[id] = (volatile unsigned int) data;
}

void axis_read_4b(unsigned int id,  int* data)
{
  volatile unsigned int r = *axis_ptr[id];
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
