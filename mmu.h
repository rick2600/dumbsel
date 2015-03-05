#ifndef __MMU_H
#define __MMU_H

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
  unsigned short int mar;
  unsigned int mdr;
} mem_bus_t;


void *mmu_service(void *args);

#endif