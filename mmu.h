#ifndef __MMU_H
#define __MMU_H

#define PAGE_SUP(x)     (((x) >> 4) & 1)
#define PAGE_READ(x)    (((x) >> 3) & 1)
#define PAGE_WRITE(x)   (((x) >> 2) & 1)
#define PAGE_PRESENT(x) ((x) & 1)
#define PAGE_BASE(x)    ((x) & ~31)

#include <stdint.h>
#include <pthread.h>

typedef enum
{
  REQ_READ,
  REQ_WRITE_W,
  REQ_WRITE_B,
  RES_READ_OK,
  RES_READ_ERR,
  RES_WRITE_OK,
  RES_WRITE_ERR,
  IDLE
} mem_control_bus_t;


typedef struct 
{ 
  pthread_mutex_t lock;
  mem_control_bus_t control;
  uint16_t mar;
  uint32_t mdr;
} mmu_t;


void *mmu_service(void *args);


#endif