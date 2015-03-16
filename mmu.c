#include "vm.h"
#include "isa.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void mmu_read(vm_t *vm);
static void mmu_write(vm_t *vm, uint32_t mode);
static uint16_t mmu_translation_unit(vm_t *vm, uint32_t *read_ok, uint32_t *write_ok);


void *mmu_service(void *args)
{
  vm_t *vm = (vm_t *)args;
  while(1)
  {  
    pthread_mutex_lock(&vm->mmu.lock);
    if (IN_HALT_STATE(vm->cpu.ccr))
    {
      pthread_mutex_unlock(&vm->mmu.lock);
      break;  
    }
    switch(vm->mmu.control)
    {
      case REQ_READ: mmu_read(vm); break;
      case REQ_WRITE_W: mmu_write(vm, REQ_WRITE_W); break;
      case REQ_WRITE_B: mmu_write(vm, REQ_WRITE_B); break;
      default: break;
    }
    pthread_mutex_unlock(&vm->mmu.lock);
  }
  return NULL;
}

static uint16_t mmu_translation_unit(vm_t *vm, uint32_t *read_ok, uint32_t *write_ok)
{
  uint16_t addr, base, index, offset, page;

  addr = vm->mmu.mar;

  if (VIRTUAL_MODE_ENABLED(vm->cpu.ccr))
  {
    index = vm->mmu.mar >> 12;
    page = *(uint16_t *)&vm->ram[vm->cpu.acr + (index * 2)];    
    base = page & ~31;
    offset = vm->mmu.mar & 0x0fff;
    addr = base + offset;

    if (!PAGE_PRESENT(page) || 
       (!IN_SUPERVISOR_MODE(vm->cpu.ccr) && PAGE_SUP(page)))
    {
      *read_ok = 0;
      *write_ok = 0;
    }
    else
    {
      if (PAGE_READ(page))
        *read_ok = 1;

      if (PAGE_WRITE(page))
        *write_ok = 1;
    }

  }
  else
  {
    *read_ok = 1; 
    *write_ok = 1; 
  }

  return addr;
}


static void mmu_read(vm_t *vm)
{
  uint32_t read_ok, write_ok;
  uint16_t paddr = mmu_translation_unit(vm, &read_ok, &write_ok);

  if (read_ok)
  {
    if (paddr < RAM_SIZE)
    {
      vm->mmu.mdr = *(uint32_t *)&vm->ram[paddr];
      vm->mmu.control = RES_READ_OK;
    }
    else
      vm->mmu.control = RES_READ_ERR;
  }

}

static void mmu_write(vm_t *vm, uint32_t mode)
{
  uint16_t value;
  uint32_t read_ok, write_ok;
  uint16_t paddr = mmu_translation_unit(vm, &read_ok, &write_ok);

  if (write_ok)
  {
    if (mode == REQ_WRITE_W)
    {   
      if (paddr < (RAM_SIZE-2))
      {
        value = (uint16_t)vm->mmu.mdr;
        *(uint16_t *)&vm->ram[paddr] = value;
        vm->mmu.control = RES_WRITE_OK;
      }
      else
        vm->mmu.control = RES_WRITE_ERR;
    }
    else
    {
      if (paddr < (RAM_SIZE-1))
      {
        value = (uint8_t)vm->mmu.mdr;
        *(uint8_t *)&vm->ram[paddr] = value;
        vm->mmu.control = RES_WRITE_OK;
      }
      else
        vm->mmu.control = RES_WRITE_ERR;
    }
  }

}
