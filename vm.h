#ifndef __VM_H
#define __VM_H

#include "cpu.h"
#include "mmu.h"
#include "io.h"
#include "ram.h"

#define RAM_SIZE 0xf000

typedef struct 
{  
  io_bus_t *io_bus;
  mmu_t mmu;
  cpu_t cpu;
  ram_t *ram;
  uint32_t debug_mode;

} vm_t;


vm_t *create_vm(void);
int turn_on(vm_t *vm, char *code_file);
int turn_off(vm_t *vm);

#endif