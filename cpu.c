#include "vm.h"
#include "cpu.h"
#include "debug.h"
#include "isa.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SHOW_REGISTER 1
#define SHOW_MEMORY   1
#define SHOW_INSTRUCTION 1
#define STOP 0

static void cpu_fetch(vm_t *vm);
static void cpu_decode(vm_t *vm);
static void cpu_execute(vm_t *vm);

static uint32_t fetch_from_cache(vm_t *vm, uint32_t *miss);
static void cache_instruction(vm_t *vm, uint32_t inst);
static uint32_t fetch_from_mem(vm_t *vm);
static void raise_interruption(vm_t *vm, cpu_int_t interruption);
static void enter_inthandler(vm_t *vm);
static void set_clean_context(vm_t *vm);

static void cpu_enter_supervisor_mode(vm_t *vm);
static void cpu_disable_interruptions(vm_t *vm);
static void cpu_enter_halt_state(vm_t *vm);


void *cpu_uc(void *args)
{
 
  vm_t *vm = (vm_t *)args;
  
  //cpu_enter_supervisor_mode(vm);
  //cpu_disable_interruptions(vm);

  while(1)
  {
    if (IN_HALT_STATE(vm->cpu.ccr))
      break;

    cpu_fetch(vm);
    cpu_decode(vm);

    if (vm->debug_mode)
      run_debugger2(vm, !STOP);

    cpu_execute(vm);

    if (INTERRUPTION_ENABLED(vm->cpu.ccr))
      vm->cpu.time_slice--;

    if (!vm->cpu.time_slice)
    {
      if (INTERRUPTION_ENABLED(vm->cpu.ccr))
        raise_interruption(vm, QUANTUM_EXPIRED);

      vm->cpu.time_slice = QUANTUM;
    }
  
  }
  return NULL;
}

static void cpu_fetch(vm_t *vm)
{
  uint32_t instruction, cache_miss = 1;
  instruction = fetch_from_cache(vm, &cache_miss);

  if (cache_miss)
  {
    instruction = fetch_from_mem(vm);
    cache_instruction(vm, instruction);
  }

  vm->cpu.pc += 4;
  vm->cpu.ir = SWAP_UINT32(instruction);
}

static uint32_t fetch_from_cache(vm_t *vm, uint32_t *miss)
{
  uint32_t i;
  for (i = 0; i < ICACHE_SIZE; i++)
    if (vm->cpu.icache_addr[i] == (uint32_t)vm->cpu.pc)
    {
      *miss = 0;
      return vm->cpu.icache_data[i];
    }

  return 0;
}

static uint32_t fetch_from_mem(vm_t *vm)
{
  uint32_t data;

  if (!cpu_r_mem(vm, vm->cpu.pc, &data))
    fprintf(stderr, "Bus error 9\n");

  return data;
}

static void cache_instruction(vm_t *vm, uint32_t inst)
{
  vm->cpu.icache_addr[vm->cpu.icache_oldest] = vm->cpu.pc;
  vm->cpu.icache_data[vm->cpu.icache_oldest] = inst;
  vm->cpu.icache_oldest++;
  if (vm->cpu.icache_oldest == 16)
    vm->cpu.icache_oldest = 0;
}

static void raise_interruption(vm_t *vm, cpu_int_t interruption)
{
  uint16_t handler;
  uint32_t addr, data; 

  addr = vm->cpu.icr + (interruption * sizeof(uint16_t));
  printf("a: %04x\n", addr);
  if (cpu_r_mem(vm, addr, &data))
  {
    handler = data & 0xffff;

    printf("INT %d, handler: %04x\n", interruption, handler); 


    enter_inthandler(vm);
    cpu_disable_interruptions(vm);
    cpu_enter_supervisor_mode(vm);
    vm->cpu.pc = handler;

  }
  else
  {
    fprintf(stderr, "Bus error\n");
  }
}

static void enter_inthandler(vm_t *vm)
{
  if (cpu_w_mem(vm, vm->cpu.regs[15] - 2, vm->cpu.flags, REQ_WRITE_W))
    vm->cpu.regs[15] -= 2;

  if (cpu_w_mem(vm, vm->cpu.regs[15] - 2, vm->cpu.ccr, REQ_WRITE_W))
    vm->cpu.regs[15] -= 2;

  if (cpu_w_mem(vm, vm->cpu.regs[15] - 2, vm->cpu.pc, REQ_WRITE_W))
    vm->cpu.regs[15] -= 2;

}


static void cpu_decode(vm_t *vm)
{
  vm->cpu.inst.op = vm->cpu.ir >> 26;
  vm->cpu.inst.i = (vm->cpu.ir >> 25) & 1;
  vm->cpu.inst.bmode = (vm->cpu.ir >> 24) & 1;
  vm->cpu.inst.ra = (vm->cpu.ir >> 20) & 0x0f;
  vm->cpu.inst.rb = (vm->cpu.ir >> 16) & 0x0f;
  vm->cpu.inst.rc = (vm->cpu.ir >> 12) & 0x0f;
  vm->cpu.inst.imm = SWAP_UINT16(vm->cpu.ir & 0xffff);
}

static void cpu_execute(vm_t *vm)
{
  if (vm->cpu.opers[vm->cpu.inst.op])
    vm->cpu.opers[vm->cpu.inst.op](vm);
  else
    cpu_enter_halt_state(vm);
}

static void cpu_enter_supervisor_mode(vm_t *vm)
{
  vm->cpu.ccr |= 4;
}

static void cpu_disable_interruptions(vm_t *vm)
{
  vm->cpu.ccr &= ~1;
}

static void cpu_enter_halt_state(vm_t *vm)
{
  vm->cpu.ccr |= 2;
}
