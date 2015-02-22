#ifndef __VM_H
#define __VM_H

#include "cpu.h"
#include "mmu.h"
#include "io.h"
#include "ram.h"

#define RAM_SIZE 256

typedef struct 
{  
  io_bus_t *io_bus;
  mem_bus_t *mem_bus;
  cpu_t *cpu;
  ram_t *ram;
  int debug_mode;

} vm_t;


vm_t *create_vm(void);
int turn_on(vm_t *vm, char *code_file);
int turn_off(vm_t *vm);

#endif