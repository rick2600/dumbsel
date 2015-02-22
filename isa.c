#include "isa.h"
#include "vm.h"
#include <stdio.h>
#include <unistd.h>

static int alu(vm_t *vm, int operands);

int isa_mov(vm_t *vm)
{
  unsigned short int temp;

  if (vm->cpu->inst->has_imm)
    temp = vm->cpu->inst->imm;
  else
    temp = vm->cpu->regs[vm->cpu->inst->rb];

  if (vm->cpu->inst->byte_mode)
    vm->cpu->regs[vm->cpu->inst->ra] = \
    (vm->cpu->regs[vm->cpu->inst->ra] & 0xff00)|(0xff & temp);
  else
    vm->cpu->regs[vm->cpu->inst->ra] = temp;

  return 1;
}

int isa_ext(vm_t *vm)
{
  vm->cpu->regs[vm->cpu->inst->ra] = vm->cpu->regs[vm->cpu->inst->rb] & 0xff;
  return 1;
}

int isa_exts(vm_t *vm)
{
  unsigned short int temp;

  temp  = vm->cpu->regs[vm->cpu->inst->rb] & 0xff;
  if (temp & 0x80)
    vm->cpu->regs[vm->cpu->inst->ra] = temp | 0xff00;
  else
    vm->cpu->regs[vm->cpu->inst->ra] = temp;

  return 1;
}


int isa_add(vm_t *vm)
{
  return alu(vm, 2);
}

int isa_sub(vm_t *vm)
{
  return alu(vm, 2);
}

int isa_mul(vm_t *vm)
{
  return alu(vm, 2);
}

int isa_div(vm_t *vm)
{
  return alu(vm, 2);
}

int isa_inc(vm_t *vm)
{
  return alu(vm, 1);
}

int isa_dec(vm_t *vm)
{
  return alu(vm, 1);
}

int isa_not(vm_t *vm)
{
  return alu(vm, 1);
}

int isa_or(vm_t *vm)
{
  return alu(vm, 2);
}

int isa_xor(vm_t *vm)
{
  return alu(vm, 2);
}

int isa_and(vm_t *vm)
{
  return alu(vm, 2);
}

int isa_shl(vm_t *vm)
{
  return alu(vm, 2);
}

int isa_shr(vm_t *vm)
{
  return alu(vm, 2);
}

int isa_hlt(vm_t *vm)
{
  vm->cpu->halt = 1;
  return 1;
}

int isa_pop(vm_t *vm)
{
  unsigned short int temp;
  pthread_mutex_lock(&vm->mem_bus->lock);
  vm->mem_bus->addr = vm->cpu->regs[15];
  vm->mem_bus->control = REQ_READ;
  pthread_mutex_unlock(&vm->mem_bus->lock);
  usleep(2000);
  pthread_mutex_lock(&vm->mem_bus->lock);
  if (RES_READ_OK)
  {
    temp = vm->mem_bus->data & 0xffff;
    vm->cpu->regs[vm->cpu->inst->ra] = temp ;
    vm->cpu->regs[15] += 2;
  }
  else
  {
    fprintf(stderr, "Bus Error\n");
    turn_off(vm);
    //TODO: write a recovery code?
  }
  pthread_mutex_unlock(&vm->mem_bus->lock);
  return 1;
}

int isa_load(vm_t *vm)
{
  unsigned short int temp = vm->cpu->regs[vm->cpu->inst->rb];

  if (vm->cpu->inst->has_imm)
    temp += vm->cpu->inst->imm;
  else
    temp += vm->cpu->regs[vm->cpu->inst->rc];

  pthread_mutex_lock(&vm->mem_bus->lock);
  vm->mem_bus->addr = temp;
  vm->mem_bus->control = REQ_READ;
  pthread_mutex_unlock(&vm->mem_bus->lock);
  usleep(2000);
  pthread_mutex_lock(&vm->mem_bus->lock);
  if (RES_READ_OK)
  {
    temp = vm->mem_bus->data & 0xffff;

    if (vm->cpu->inst->byte_mode)
      vm->cpu->regs[vm->cpu->inst->ra] = \
      (vm->cpu->regs[vm->cpu->inst->ra] & 0xff00)|(0xff & temp);
    else
      vm->cpu->regs[vm->cpu->inst->ra] = temp;
  }
  else
  {
    fprintf(stderr, "Bus Error\n");
    turn_off(vm);
    //TODO: write a recovery code?
  }
  pthread_mutex_unlock(&vm->mem_bus->lock);
}

int isa_store(vm_t *vm)
{
  unsigned short int temp = vm->cpu->regs[vm->cpu->inst->rb];

  if (vm->cpu->inst->has_imm)
    temp += vm->cpu->inst->imm;
  else
    temp += vm->cpu->regs[vm->cpu->inst->rc];

  pthread_mutex_lock(&vm->mem_bus->lock);
  vm->mem_bus->data = vm->cpu->regs[vm->cpu->inst->ra];
  vm->mem_bus->addr = temp;
  vm->mem_bus->control = (vm->cpu->inst->byte_mode) ? REQ_WRITE_B : REQ_WRITE_W;
  pthread_mutex_unlock(&vm->mem_bus->lock);
  usleep(2000);
  pthread_mutex_lock(&vm->mem_bus->lock);
  if (!RES_WRITE_OK)
  {
    fprintf(stderr, "Bus Error\n");
    turn_off(vm);
    //TODO: write a recovery code?
  }
  pthread_mutex_unlock(&vm->mem_bus->lock);
}


static int alu(vm_t *vm, int operands)
{
  unsigned short int op0, op1, res;
  if (operands == 2)
  {
    op0 = vm->cpu->regs[vm->cpu->inst->rb];

    if (vm->cpu->inst->has_imm)
      op1 = vm->cpu->inst->imm;
    else
      op1 = vm->cpu->regs[vm->cpu->inst->rc];

    if (vm->cpu->inst->byte_mode)
    {
      op0 &= 0xff;
      op1 &= 0xff;
    }
  
    switch(vm->cpu->inst->op)
    {
      case ADD: res = op0 + op1; break;
      case SUB: res = op0 - op1; break;
      case DIV: res = op0 / op1; break;
      case MUL: res = op0 * op1; break;
      case OR:  res = op0 | op1; break;
      case XOR: res = op0 ^ op1; break;
      case AND: res = op0 & op1; break;
      case SHL: res = op0 << op1; break;
      case SHR: res = op0 >> op1; break;
      default: break;
    }
  }
  else if (operands == 1)
  {
    res = vm->cpu->regs[vm->cpu->inst->ra];
    if (vm->cpu->inst->byte_mode)
      res = (res & 0xff00) | (res & 0xff);

    switch(vm->cpu->inst->op)
    {
      case INC: res++; break;
      case DEC: res--; break;
      case NOT: res = -res; break;
      default: break;
    }
  }

  if (vm->cpu->inst->byte_mode)
    res = (vm->cpu->regs[vm->cpu->inst->ra] & 0xff00) | (res & 0xff);
  
  vm->cpu->regs[vm->cpu->inst->ra] = res;

  return 1;
}