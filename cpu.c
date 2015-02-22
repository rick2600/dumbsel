#include "vm.h"
#include "cpu.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SHOW_REGISTER 1
#define SHOW_MEMORY   1
#define SHOW_INSTRUCTION 1
#define STOP 0

static void cpu_fetch(vm_t *vm);
static void cpu_decode(vm_t *vm);
static void cpu_execute(vm_t *vm);

void *cpu_uc(void *args)
{
  vm_t *vm = (vm_t *)args;

  vm->cpu->halt = 0;

  while(1)
  {
    if (vm->cpu->halt)
      break;

    cpu_fetch(vm);
    cpu_decode(vm);

    if (vm->debug_mode)
      run_debugger(vm, SHOW_REGISTER, SHOW_MEMORY, SHOW_INSTRUCTION, 1);

    cpu_execute(vm);
  }
  return NULL;
}


static void cpu_fetch(vm_t *vm)
{
  pthread_mutex_lock(&vm->mem_bus->lock);
  vm->mem_bus->addr = vm->cpu->pc;  
  vm->mem_bus->control = REQ_READ;
  vm->cpu->pc += 4;  
  pthread_mutex_unlock(&vm->mem_bus->lock);
  usleep(2000);

  pthread_mutex_lock(&vm->mem_bus->lock);
  if (RES_READ_OK)
    vm->cpu->ir = SWAP_UINT32(vm->mem_bus->data);
  else
  {
    fprintf(stderr, "Bus Error\n");
    turn_off(vm);
    //TODO: write a recovery code?
  }
  pthread_mutex_unlock(&vm->mem_bus->lock);

}

static void cpu_decode(vm_t *vm)
{
  vm->cpu->inst->op = vm->cpu->ir >> 26;
  vm->cpu->inst->has_imm = (vm->cpu->ir >> 25) & 1;
  vm->cpu->inst->byte_mode = (vm->cpu->ir >> 24) & 1;
  vm->cpu->inst->ra = (vm->cpu->ir >> 20) & 0x0f;
  vm->cpu->inst->rb = (vm->cpu->ir >> 16) & 0x0f;
  vm->cpu->inst->rc = (vm->cpu->ir >> 12) & 0x0f;
  vm->cpu->inst->imm = SWAP_UINT16(vm->cpu->ir & 0xffff);
}

static void cpu_execute(vm_t *vm)
{
  if (vm->cpu->isa[vm->cpu->inst->op])
    vm->cpu->isa[vm->cpu->inst->op](vm);
}