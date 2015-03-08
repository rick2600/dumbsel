#include "vm.h"
#include "isa.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static short int mmu_translation_unit(vm_t *vm);
static void mmu_read(vm_t *vm);
static void mmu_write(vm_t *vm, unsigned int mode);

void *mmu_service(void *args)
{
  vm_t *vm = (vm_t *)args;
  while(1)
  {  
    pthread_mutex_lock(&vm->mem_bus->lock);
    if (IN_HALT_STATE)
    {
      pthread_mutex_unlock(&vm->mem_bus->lock);
      break;  
    }
    switch(vm->mem_bus->control)
    {
      case REQ_READ: mmu_read(vm); break;
      case REQ_WRITE_W: mmu_write(vm, REQ_WRITE_W); break;
      case REQ_WRITE_B: mmu_write(vm, REQ_WRITE_B); break;
      default: break;
    }
    pthread_mutex_unlock(&vm->mem_bus->lock);
  }
  return NULL;
}

static short int mmu_translation_unit(vm_t *vm)
{
  unsigned short int addr = vm->mem_bus->mar, base, index, offset;
  unsigned short int ptable = vm->ram[vm->cpu->acr];

/*
  printf("v: %04x\n", addr);
  if (VIRTUAL_MODE_ENABLED)
  {
    index = vm->mem_bus->mar >> 12;
    base = *(unsigned short int *)&vm->ram[ptable + (index * 2)];
    offset = vm->mem_bus->mar & 0x0fff;
    addr = base + offset;
    printf("b: %04x o: %04x => %04x\n", base, offset, addr);
  }
*/
  return addr;
}


static void mmu_read(vm_t *vm)
{
  unsigned short int paddr = mmu_translation_unit(vm);

  if (paddr < RAM_SIZE)
  {
    vm->mem_bus->mdr = *(unsigned int *)&vm->ram[paddr];
    vm->mem_bus->control = RES_READ_OK;
  }
  else
    vm->mem_bus->control = RES_READ_ERR;

}

static void mmu_write(vm_t *vm, unsigned int mode)
{
  unsigned short int value;
  unsigned short int paddr = mmu_translation_unit(vm);

  if (mode == REQ_WRITE_W)
  {   
    if (paddr < (RAM_SIZE-2))
    {
      value = (unsigned short int)vm->mem_bus->mdr;
      *(unsigned short int *)&vm->ram[paddr] = value;
      vm->mem_bus->control = RES_WRITE_OK;
    }
    else
      vm->mem_bus->control = RES_WRITE_ERR;
  }
  else
  {
    if (paddr < (RAM_SIZE-1))
    {
      value = (unsigned char)vm->mem_bus->mdr;
      *(unsigned char *)&vm->ram[paddr] = value;
      vm->mem_bus->control = RES_WRITE_OK;
    }
    else
      vm->mem_bus->control = RES_WRITE_ERR;
  }

}
