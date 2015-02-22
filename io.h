#ifndef __IO_H
#define __IO_H

#include <pthread.h>

typedef struct 
{
  pthread_mutex_t lock;
} io_bus_t;

#endif