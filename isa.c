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
  ENTER_HALT_STATE;
  return 1;
}

int isa_psh(vm_t *vm)
{
  unsigned short int addr, data;

  if (vm->cpu->inst->has_imm)
    data = vm->cpu->inst->imm;
  else
    data = vm->cpu->regs[vm->cpu->inst->ra];

  addr = vm->cpu->regs[15] - 2;

  if (cpu_w_mem(vm, addr, data, REQ_WRITE_W))
    vm->cpu->regs[15] -= 2;
  else
  {
    fprintf(stderr, "Bus error\n");
  }

  return 1;
}

int isa_psha(vm_t *vm)
{
  int i;
  unsigned short int addr, data;

  for (i = 15; i >=0; i--)
  {
    addr = vm->cpu->regs[15] - 2;
    data = vm->cpu->regs[i];
    if(cpu_w_mem(vm, addr, data, REQ_WRITE_W))
      vm->cpu->regs[15] -= 2;
    else
    {
      fprintf(stderr, "Bus error\n");
    }
  }
  
  return 1;
}


int isa_popa(vm_t *vm)
{
  unsigned short int i, addr;
  unsigned int data;
  for (i = 0; i < 15; i++)
  {
    addr = vm->cpu->regs[15];
    if (cpu_r_mem(vm, addr, &data))
    {
      vm->cpu->regs[i] = data & 0xffff;
      vm->cpu->regs[15] += 2;
    }
    else
    {
      fprintf(stderr, "Bus error\n");
    }
  }
  vm->cpu->regs[15] += 2;
  return 1;
}


int isa_iback(vm_t *vm)
{
  /*
  for (i = 0; i < 16; i++)
    vm->cpu->regs[i] = vm->cpu->_regs[i];

  vm->cpu->pc = vm->cpu->_pc;
  vm->cpu->flags = vm->cpu->_flags;
  vm->cpu->ccr = CCR_INT_ENABLE(vm->cpu->ccr);
  vm->cpu->time_slice = 0;
  */
  return 1;
}

int isa_pop(vm_t *vm)
{
  unsigned short int addr;
  unsigned int data;

  addr = vm->cpu->regs[15];

  if (cpu_r_mem(vm, addr, &data))
  {
    vm->cpu->regs[vm->cpu->inst->ra] = data & 0xffff;
    vm->cpu->regs[15] += 2;
  }
  else
  {
    fprintf(stderr, "Bus error\n");
  }
  return 1;
}

int isa_load(vm_t *vm)
{
  unsigned short int addr, temp;
  unsigned int data;

  addr = vm->cpu->regs[vm->cpu->inst->rb];

  if (vm->cpu->inst->has_imm)
    addr += vm->cpu->inst->imm;
  else
    addr += vm->cpu->regs[vm->cpu->inst->rc];

  if (cpu_r_mem(vm, addr, &data))
  {
    temp = data & 0xffff;
    if (vm->cpu->inst->byte_mode)
      vm->cpu->regs[vm->cpu->inst->ra] = \
      (vm->cpu->regs[vm->cpu->inst->ra] & 0xff00)|(0xff & temp);
    else
      vm->cpu->regs[vm->cpu->inst->ra] = temp;
  }
  else
  {
    fprintf(stderr, "Bus error\n");
  }
  return 1;
}

int isa_store(vm_t *vm)
{
  unsigned short int addr, data;
  unsigned int mode_req;
  addr = vm->cpu->regs[vm->cpu->inst->rb];

  if (vm->cpu->inst->has_imm)
    addr += vm->cpu->inst->imm;
  else
    addr += vm->cpu->regs[vm->cpu->inst->rc];

  data = vm->cpu->regs[vm->cpu->inst->ra];

  mode_req = (vm->cpu->inst->byte_mode) ? REQ_WRITE_B : REQ_WRITE_W;

  if (!cpu_w_mem(vm, addr, data, mode_req))
  {
    fprintf(stderr, "Bus error\n");
  }
  return 1;
}


int isa_cmp(vm_t *vm)
{
  unsigned short int op0 = vm->cpu->regs[vm->cpu->inst->ra], op1;
  if (vm->cpu->inst->has_imm)
    op1 = vm->cpu->inst->imm;
  else
    op1 = vm->cpu->regs[vm->cpu->inst->rb];

  if (vm->cpu->inst->byte_mode)
  {
    op0 &= 0xff;
    op1 &= 0xff;
  }

  (op0 == op1) ? SET_ZF : CLR_ZF;
  (op0 < op1)  ? SET_LT : CLR_LT;
  (op0 > op1)  ? SET_GT : CLR_GT;
  
  return 1;
}


int isa_cmps(vm_t *vm)
{
  signed short int op0, op1;
  signed char op0b, op1b;

  op0 = (signed short int)vm->cpu->regs[vm->cpu->inst->ra];

  if (vm->cpu->inst->has_imm)
    op1 = (signed short int)vm->cpu->inst->imm;
  else
    op1 = (signed short int)vm->cpu->regs[vm->cpu->inst->rb];

  if (vm->cpu->inst->byte_mode)
  {
    op0b = (signed char)op0;
    op1b = (signed char)op1;
    (op0b == op1b) ? SET_ZF : CLR_ZF;
    (op0b < op1b)  ? SET_LT : CLR_LT;
    (op0b > op1b)  ? SET_GT : CLR_GT;
  }
  else
  {    
    (op0 == op1) ? SET_ZF : CLR_ZF;
    (op0 < op1)  ? SET_LT : CLR_LT;
    (op0 > op1)  ? SET_GT : CLR_GT;  
  }

  return 1;
}


int isa_nop(vm_t *vm)
{
  return 1;
}

int isa_br(vm_t *vm)
{
  if (vm->cpu->inst->has_imm)
    vm->cpu->pc += vm->cpu->inst->imm;
  else
    vm->cpu->pc = vm->cpu->regs[vm->cpu->inst->ra];

  return 1;
}

int isa_bre(vm_t *vm)
{
  if (ZF)
  {
    if (vm->cpu->inst->has_imm)
      vm->cpu->pc += vm->cpu->inst->imm;
    else
      vm->cpu->pc = vm->cpu->regs[vm->cpu->inst->ra];
  }
  return 1;
}

int isa_brne(vm_t *vm)
{
  if (!ZF)
  {
    if (vm->cpu->inst->has_imm)
      vm->cpu->pc += vm->cpu->inst->imm;
    else
      vm->cpu->pc = vm->cpu->regs[vm->cpu->inst->ra];
  }
  return 1;
}

int isa_brg(vm_t *vm)
{
  if (GT)
  {
    if (vm->cpu->inst->has_imm)
      vm->cpu->pc += vm->cpu->inst->imm;
    else
      vm->cpu->pc = vm->cpu->regs[vm->cpu->inst->ra];
  }
  return 1;
}

int isa_brge(vm_t *vm)
{
  if (GT || ZF)
  {
    if (vm->cpu->inst->has_imm)
      vm->cpu->pc += vm->cpu->inst->imm;
    else
      vm->cpu->pc = vm->cpu->regs[vm->cpu->inst->ra];
  }
  return 1;
}

int isa_brl(vm_t *vm)
{
  if (LT)
  {
    if (vm->cpu->inst->has_imm)
      vm->cpu->pc += vm->cpu->inst->imm;
    else
      vm->cpu->pc = vm->cpu->regs[vm->cpu->inst->ra];
  }
  return 1;
}

int isa_brle(vm_t *vm)
{
  if (LT || ZF)
  {
    if (vm->cpu->inst->has_imm)
      vm->cpu->pc += vm->cpu->inst->imm;
    else
      vm->cpu->pc = vm->cpu->regs[vm->cpu->inst->ra];
  }
  return 1;
}

int isa_call(vm_t *vm)
{
  if (cpu_w_mem(vm, vm->cpu->regs[15] - 2, vm->cpu->pc, REQ_WRITE_W))
  {
    vm->cpu->regs[15] -= 2;
    if (vm->cpu->inst->has_imm)
      vm->cpu->pc += vm->cpu->inst->imm;
    else
      vm->cpu->pc = vm->cpu->regs[vm->cpu->inst->ra];
  }
  else
  {
    fprintf(stderr, "Bus error\n");
  }
  return 1;
}

int isa_back(vm_t *vm)
{
  unsigned int data;
  if (cpu_r_mem(vm, vm->cpu->regs[15], &data))
  {
    vm->cpu->pc = data & 0xffff;
    vm->cpu->regs[15] += 2;
  }
  else
  {
    fprintf(stderr, "Bus error\n");
  }
  return 1;
}

int isa_ldflg(vm_t *vm)
{
  vm->cpu->regs[0] = vm->cpu->flags;
  return 1;
}

int isa_stflg(vm_t *vm)
{
  vm->cpu->flags = vm->cpu->regs[0];
  return 1;
}

int isa_ldccr(vm_t *vm)
{
  vm->cpu->regs[0] = vm->cpu->ccr;
  return 1;
}

int isa_stccr(vm_t *vm)
{
  vm->cpu->ccr = vm->cpu->regs[0];
  return 1;
}

int isa_ldicr(vm_t *vm)
{
  vm->cpu->regs[0] = vm->cpu->icr;
  return 1;
}

int isa_sticr(vm_t *vm)
{
  vm->cpu->icr = vm->cpu->regs[0];
  return 1;
}

int isa_ldtcr(vm_t *vm)
{
  vm->cpu->regs[0] = vm->cpu->tcr;
  return 1;
}

int isa_sttcr(vm_t *vm)
{
  vm->cpu->tcr = vm->cpu->regs[0];
  return 1;
}

int isa_ldctx(vm_t *vm)
{

  unsigned short i, addr = vm->cpu->regs[0];
  unsigned int data;

  for (i = 0; i < ICACHE_SIZE; i++)
    vm->cpu->icache_addr[i] = -1;

  cpu_r_mem(vm, addr + 2, &data);
  vm->cpu->flags = data & 0xffff;

  cpu_r_mem(vm, addr + 4, &data);
  vm->cpu->acr = data & 0xffff;

  cpu_r_mem(vm, addr + 6, &data);
  vm->cpu->pc = data & 0xffff;

  for (i = 0; i < 16; i++)
  {
    cpu_r_mem(vm, addr + 8 + (i*2), &data);
    vm->cpu->regs[i] = data & 0xffff;
  }

  ENTER_SUPERVISOR_MODE;
  vm->cpu->time_slice = 0;
  return 1;
}

int isa_stctx(vm_t *vm)
{
  unsigned short int i;

  cpu_w_mem(vm, vm->cpu->regs[0] + 2, vm->cpu->_flags, REQ_WRITE_W);
  cpu_w_mem(vm, vm->cpu->regs[0] + 4, vm->cpu->acr, REQ_WRITE_W);
  cpu_w_mem(vm, vm->cpu->regs[0] + 6, vm->cpu->_pc, REQ_WRITE_W);
  for (i = 0; i < 16; i++)
    cpu_w_mem(vm, vm->cpu->regs[0] + 8+(i*2), vm->cpu->_regs[i], REQ_WRITE_W);

  return 1;
}


int isa_di(vm_t *vm)
{
  vm->cpu->ccr = vm->cpu->ccr & 0xfffe;
  return 1;
}

int isa_ei(vm_t *vm)
{
  vm->cpu->ccr = vm->cpu->ccr | 0x0001;
  vm->cpu->time_slice = 0;
  return 1;
}

static int alu(vm_t *vm, int operands)
{
  unsigned short int op0, op1, res;
  unsigned short int sign0, sign1, sign2, cf;


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
      sign0 = op0 & 0x80;
      sign1 = op1 & 0x80;
    }
    else
    {
      sign0 = op0 & 0x8000;
      sign1 = op1 & 0x8000;
    }
  
    switch(vm->cpu->inst->op)
    {
      case ADD:
      {
        res = op0 + op1;
        sign2 = (vm->cpu->inst->byte_mode) ? (res & 0x80) : (res & 0x8000);
        ((sign0 == sign1) && (sign0 != sign2)) ? SET_OF : CLR_OF;
      }
      break;

      case SUB:
      {
        res = op0 - op1;
        sign2 = (vm->cpu->inst->byte_mode) ? (res & 0x80) : (res & 0x8000);
        ((sign0 == sign1) && (sign0 != sign2)) ? SET_OF : CLR_OF;
      } 
      break;

      case DIV:
      {
        if (op1 != 0)
        {
          res = op0 / op1;
          sign2 = (vm->cpu->inst->byte_mode) ? (res & 0x80) : (res & 0x8000);
          ((sign0 == sign1) && (sign0 != sign2)) ? SET_OF : CLR_OF;
        }
        else
        {
          res = 0;
          // raise exception
        } 
      }
      break;

      case MUL:
      {
        res = op0 * op1; 
        sign2 = (vm->cpu->inst->byte_mode) ? (res & 0x80) : (res & 0x8000);
        ((sign0 == sign1) && (sign0 != sign2)) ? SET_OF : CLR_OF;
      }
      break;

      case OR:
      {
        CLR_CF;
        res = op0 | op1;
      } 
      break;

      case XOR:
      {
        res = op0 ^ op1;
      }
      break;

      case AND:
      {
        res = op0 & op1;
      }
      break;

      case SHL: 
      {
        //res = op0 << op1;
        while(op1)
        {
          cf = (vm->cpu->inst->byte_mode) ? (op0 & 0x80) : (op0 & 0x8000);
          op0 <<= 1;
          op1--;
        }
        res = op0;
        (cf) ? SET_CF : CLR_CF;
      }
      break;

      case SHR:
      {
        res = op0 >> op1; 
      }
      break;

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
      case NOT: res = ~res; break;
      default: break;
    }
  }

  if (vm->cpu->inst->byte_mode)
  {
    (res & 0x80) ? SET_SF : CLR_SF;
    res = (vm->cpu->regs[vm->cpu->inst->ra] & 0xff00) | (res & 0xff);
  }
  else
  {
    (res & 0x8000) ? SET_SF : CLR_SF;
  }
  
  (res == 0) ? SET_ZF : CLR_ZF;

  vm->cpu->regs[vm->cpu->inst->ra] = res;
  return 1;
}


int cpu_w_mem(vm_t *vm, unsigned short int mar,
                  unsigned int mdr, unsigned int m)
{
  int retval = 0;

  pthread_mutex_lock(&vm->mem_bus->lock);
  vm->mem_bus->mdr = mdr;
  vm->mem_bus->mar = mar;
  vm->mem_bus->control = m;
  pthread_mutex_unlock(&vm->mem_bus->lock);

  usleep(2000);

  pthread_mutex_lock(&vm->mem_bus->lock);
  if (vm->mem_bus->control == RES_WRITE_OK)
    retval = 1;
  pthread_mutex_unlock(&vm->mem_bus->lock);
  return retval;
}


int cpu_r_mem(vm_t *vm, unsigned short int mar, unsigned int *data)
{
  int retval = 0;

  pthread_mutex_lock(&vm->mem_bus->lock);
  vm->mem_bus->mar = mar;
  vm->mem_bus->control = REQ_READ;
  pthread_mutex_unlock(&vm->mem_bus->lock);

  usleep(2000);

  pthread_mutex_lock(&vm->mem_bus->lock);
  if (vm->mem_bus->control == RES_READ_OK)
  {
    *data = vm->mem_bus->mdr;
    retval = 1;
  }
  pthread_mutex_unlock(&vm->mem_bus->lock);
  return retval;
}