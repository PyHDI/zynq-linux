#ifndef CMA_H
#define CMA_H

#define DEV_CMA "/dev/mem"
// 256 MB memory
#define CMA_OFFSET (0x07c00000)
// 512 MB memory
//#define CMA_OFFSET (0x0ec00000)
// 128 MB
//#define CMA_SIZE   (0x08000000)
// 32 MB
#define CMA_SIZE   (0x02000000)

#define CMA_PAGE_SIZE (4*1024)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <fcntl.h>

int fd_cma;
volatile char* cma_ptr = NULL;
unsigned int cma_used = 0;

void cma_open()
{
  fd_cma = open(DEV_CMA, O_RDWR | O_SYNC);
  if(fd_cma < 1){
    printf("cma_open(): Invalid device file: '%s'\n", DEV_CMA);
    exit(1);
  }
  cma_ptr = (volatile char*) mmap(NULL, CMA_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd_cma, CMA_OFFSET);
  if(cma_ptr == MAP_FAILED){
    printf("cma_open(): mmap failed.\n");
    exit(1);
  }
  cma_used = 0;
}

void* cma_malloc(unsigned int bytes)
{
  if(cma_ptr == NULL){
    printf("cma_malloc(): CMA is not opened.\n");
    return NULL;
  }

  unsigned int numpages = bytes / CMA_PAGE_SIZE;
  if(bytes % CMA_PAGE_SIZE != 0){
    numpages++;
  }

  unsigned int size = CMA_PAGE_SIZE*numpages;

  if(cma_used + size > CMA_SIZE){
    return NULL;
  }

  void* ptr = (void*)(cma_ptr + cma_used);
  cma_used += size;
  return ptr;
}

unsigned int cma_get_physical_address(void* ptr)
{
  return CMA_OFFSET + ((unsigned int) ptr) - ((unsigned int)cma_ptr);
}

void cma_cache_clean(char* addr, unsigned int bytes)
{
  //__clear_cache(addr, addr + bytes);
  //msync((void*)addr, bytes, MS_SYNC);
}

void cma_close()
{
  if(cma_ptr == NULL){
    printf("cma_close(): CMA is not opened.\n");
    return;
  }
  munmap((void*) cma_ptr, CMA_SIZE);
  cma_ptr = NULL;
}

#endif
