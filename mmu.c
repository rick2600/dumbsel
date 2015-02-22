#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void mmu_read(vm_t *vm);
static void mmu_write(vm_t *vm);

void *mmu_service(void *args)
{
  vm_t *vm = (vm_t *)args;
  while(1)
  {
    pthread_mutex_lock(&vm->mem_bus->lock);
    switch(vm->mem_bus->control)
    {
      case REQ_READ: mmu_read(vm); break;
      case REQ_WRITE: mmu_write(vm); break;
      default: break;
    }
    pthread_mutex_unlock(&vm->mem_bus->lock);
  }
}

static void mmu_read(vm_t *vm)
{
  if (vm->mem_bus->addr < RAM_SIZE)
  {
    vm->mem_bus->data = *(unsigned int *)&vm->ram[vm->mem_bus->addr];
    vm->mem_bus->control = RES_READ_OK;
  }
  else
    vm->mem_bus->control = RES_READ_ERR;

}

static void mmu_write(vm_t *vm)
{
  unsigned short int value;
  if (vm->mem_bus->addr < RAM_SIZE)
  {
    value = (unsigned short int)vm->mem_bus->data;
    *(unsigned short int *)&vm->ram[vm->mem_bus->addr] = value;
    vm->mem_bus->control = RES_WRITE_OK;
  }
  else
    vm->mem_bus->control = RES_WRITE_ERR;
}