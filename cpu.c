#include "vm.h"
#include "cpu.h"
#include "debug.h"
#include "isa.h"
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

static unsigned int fetch_from_cache(vm_t *vm, unsigned int *miss);
static void cache_instruction(vm_t *vm, unsigned int inst);
static unsigned int fetch_from_mem(vm_t *vm);
static void raise_interruption(vm_t *vm, cpu_int_t interruption);




void *cpu_uc(void *args)
{
 
  vm_t *vm = (vm_t *)args;
  
  vm->cpu->time_slice = 0;
  vm->cpu->ccr = CCR_CLR_HALT(vm->cpu->ccr);

  while(1)
  {
    if (CCR_HALT(vm->cpu->ccr))
      break;

    cpu_fetch(vm);
    cpu_decode(vm);

    if (vm->debug_mode)
      run_debugger2(vm, !STOP);
      //run_debugger(vm, SHOW_REGISTER, SHOW_MEMORY, SHOW_INSTRUCTION, !STOP);
    //sleep(1);
    cpu_execute(vm);

    
    vm->cpu->time_slice++;
    if (vm->cpu->time_slice == 5)
    {
      vm->cpu->time_slice = 0;
      raise_interruption(vm, INT_TIME_EXPIRATION);
    }
  }
  return NULL;
}

static void cpu_fetch(vm_t *vm)
{
  unsigned int instruction, cache_miss = 1;
  instruction = fetch_from_cache(vm, &cache_miss);

  if (cache_miss)
  {
    printf("CACHE_MISS (caching...)\n");
    instruction = fetch_from_mem(vm);
    cache_instruction(vm, instruction);
  }
  else
    printf("CACHE_HIT\n");
  vm->cpu->pc += 4;
  vm->cpu->ir = SWAP_UINT32(instruction);
}

static unsigned int fetch_from_cache(vm_t *vm, unsigned int *miss)
{
  int i;
  for (i = 0; i < ICACHE_SIZE; i++)
    if (vm->cpu->icache_addr[i] == (unsigned int)vm->cpu->pc)
    {
      *miss = 0;
      return vm->cpu->icache_data[i];
    }

  return 0;
}

static unsigned int fetch_from_mem(vm_t *vm)
{
  unsigned int data;
  pthread_mutex_lock(&vm->mem_bus->lock);
  vm->mem_bus->addr = vm->cpu->pc;  
  vm->mem_bus->control = REQ_READ; 
  pthread_mutex_unlock(&vm->mem_bus->lock);
  usleep(2000);

  pthread_mutex_lock(&vm->mem_bus->lock);
  if (RES_READ_OK)
    data = vm->mem_bus->data;
  else
  {
    fprintf(stderr, "Bus Error\n");
    turn_off(vm);
    //TODO: write a recovery code?
  }
  pthread_mutex_unlock(&vm->mem_bus->lock);
  return data;
}

static void cache_instruction(vm_t *vm, unsigned int inst)
{
  //printf("caching... %04x %08x at %d\n", vm->cpu->pc, SWAP_UINT32(inst), vm->cpu->icache_oldest);

  vm->cpu->icache_addr[vm->cpu->icache_oldest] = vm->cpu->pc;
  vm->cpu->icache_data[vm->cpu->icache_oldest] = inst;
  vm->cpu->icache_oldest++;
  if (vm->cpu->icache_oldest == 16)
    vm->cpu->icache_oldest = 0;
}

static void raise_interruption(vm_t *vm, cpu_int_t interruption)
{
  unsigned short int handler;

  if (!(vm->cpu->ccr & 1))
    return;

  pthread_mutex_lock(&vm->mem_bus->lock);
  vm->mem_bus->addr = vm->cpu->icr + (interruption * sizeof(unsigned short)); 
  vm->mem_bus->control = REQ_READ; 
  pthread_mutex_unlock(&vm->mem_bus->lock);
  usleep(2000);

  pthread_mutex_lock(&vm->mem_bus->lock);
  if (RES_READ_OK)
    handler = vm->mem_bus->data;
  else
  {
    fprintf(stderr, "Bus Error\n");
    turn_off(vm);
    //TODO: write a recovery code?
  }
  pthread_mutex_unlock(&vm->mem_bus->lock);


  switch(interruption)
  {
    case INT_TIME_EXPIRATION:
      printf("Interruption: INT_TIME_EXPIRATION (handler: %04x)\n", handler);
      //vm->cpu->pc = handler;
    break;
    default: break;
  }
}


/*
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
*/

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
  else
    vm->cpu->ccr = CCR_SET_HALT(vm->cpu->ccr);
}