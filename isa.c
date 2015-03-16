#include "isa.h"
#include "vm.h"
#include <stdio.h>
#include <unistd.h>

static void alu(vm_t *vm, uint32_t operands);

void op_mov(vm_t *vm)
{
  uint16_t temp;

  if (vm->cpu.inst.i)
    temp = vm->cpu.inst.imm;
  else
    temp = vm->cpu.regs[vm->cpu.inst.rb];

  if (vm->cpu.inst.bmode)
    vm->cpu.regs[vm->cpu.inst.ra] = \
    (vm->cpu.regs[vm->cpu.inst.ra] & 0xff00)|(0xff & temp);
  else
    vm->cpu.regs[vm->cpu.inst.ra] = temp;
}

void op_ext(vm_t *vm)
{
  vm->cpu.regs[vm->cpu.inst.ra] = vm->cpu.regs[vm->cpu.inst.rb] & 0xff;
}

void op_exts(vm_t *vm)
{
  uint16_t temp;

  temp  = vm->cpu.regs[vm->cpu.inst.rb] & 0xff;
  if (temp & 0x80)
    vm->cpu.regs[vm->cpu.inst.ra] = temp | 0xff00;
  else
    vm->cpu.regs[vm->cpu.inst.ra] = temp;
}


void op_add(vm_t *vm)
{
  alu(vm, 2);
}

void op_sub(vm_t *vm)
{
  alu(vm, 2);
}

void op_mul(vm_t *vm)
{
  alu(vm, 2);
}

void op_div(vm_t *vm)
{
  alu(vm, 2);
}

void op_inc(vm_t *vm)
{
  alu(vm, 1);
}

void op_dec(vm_t *vm)
{
  alu(vm, 1);
}

void op_not(vm_t *vm)
{
  alu(vm, 1);
}

void op_or(vm_t *vm)
{
  alu(vm, 2);
}

void op_xor(vm_t *vm)
{
  alu(vm, 2);
}

void op_and(vm_t *vm)
{
  alu(vm, 2);
}

void op_shl(vm_t *vm)
{
  alu(vm, 2);
}

void op_shr(vm_t *vm)
{
  alu(vm, 2);
}

void op_hlt(vm_t *vm)
{
  ENTER_HALT_STATE;
}

void op_psh(vm_t *vm)
{
  uint16_t addr, data;

  if (vm->cpu.inst.i)
    data = vm->cpu.inst.imm;
  else
    data = vm->cpu.regs[vm->cpu.inst.ra];

  addr = vm->cpu.regs[15] - 2;

  if (cpu_w_mem(vm, addr, data, REQ_WRITE_W))
    vm->cpu.regs[15] -= 2;
  else
  {
    fprintf(stderr, "Bus error 6\n");
  }

}

void op_psha(vm_t *vm)
{
  int32_t i;
  uint16_t addr, data;

  for (i = 15; i >=0; i--)
  {
    addr = vm->cpu.regs[15] - 2;
    data = vm->cpu.regs[i];
    if(cpu_w_mem(vm, addr, data, REQ_WRITE_W))
      vm->cpu.regs[15] -= 2;
    else
    {
      fprintf(stderr, "Bus error 7\n");
    }
  }
  
}


void op_popa(vm_t *vm)
{
  uint16_t i, addr;
  uint32_t data;
  for (i = 0; i < 15; i++)
  {
    addr = vm->cpu.regs[15];
    if (cpu_r_mem(vm, addr, &data))
    {
      vm->cpu.regs[i] = data & 0xffff;
      vm->cpu.regs[15] += 2;
    }
    else
    {
      fprintf(stderr, "Bus error 8\n");
    }
  }
  vm->cpu.regs[15] += 2;
}


void op_iback(vm_t *vm)
{
  uint32_t data;

  if (cpu_r_mem(vm, vm->cpu.regs[15], &data))
  {
    vm->cpu.pc = data & 0xffff;
    vm->cpu.regs[15] += 2;
  }

  if (cpu_r_mem(vm, vm->cpu.regs[15], &data))
  {
    vm->cpu.ccr = data & 0xffff;
    vm->cpu.regs[15] += 2;
  }

  if (cpu_r_mem(vm, vm->cpu.regs[15], &data))
  {
    vm->cpu.flags = data & 0xffff;
    vm->cpu.regs[15] += 2;
  }

}

void op_pop(vm_t *vm)
{
  uint16_t addr;
  uint32_t data;

  addr = vm->cpu.regs[15];

  if (cpu_r_mem(vm, addr, &data))
  {
    vm->cpu.regs[vm->cpu.inst.ra] = data & 0xffff;
    vm->cpu.regs[15] += 2;
  }
  else
  {
    fprintf(stderr, "Bus error 1\n");
  }
}

void op_load(vm_t *vm)
{
  uint16_t addr, temp;
  uint32_t data;

  addr = vm->cpu.regs[vm->cpu.inst.rb];

  if (vm->cpu.inst.i)
    addr += vm->cpu.inst.imm;
  else
    addr += vm->cpu.regs[vm->cpu.inst.rc];

  if (cpu_r_mem(vm, addr, &data))
  {
    temp = data & 0xffff;
    if (vm->cpu.inst.bmode)
      vm->cpu.regs[vm->cpu.inst.ra] = \
      (vm->cpu.regs[vm->cpu.inst.ra] & 0xff00)|(0xff & temp);
    else
      vm->cpu.regs[vm->cpu.inst.ra] = temp;
  }
  else
  {
    fprintf(stderr, "Bus error 2\n");
  }
}

void op_store(vm_t *vm)
{
  uint16_t addr, data;
  uint32_t mode_req;
  addr = vm->cpu.regs[vm->cpu.inst.rb];

  if (vm->cpu.inst.i)
    addr += vm->cpu.inst.imm;
  else
    addr += vm->cpu.regs[vm->cpu.inst.rc];

  data = vm->cpu.regs[vm->cpu.inst.ra];

  mode_req = (vm->cpu.inst.bmode) ? REQ_WRITE_B : REQ_WRITE_W;

  if (!cpu_w_mem(vm, addr, data, mode_req))
  {
    fprintf(stderr, "Bus error 3\n");
  }
}


void op_cmp(vm_t *vm)
{
  uint16_t op0 = vm->cpu.regs[vm->cpu.inst.ra], op1;
  if (vm->cpu.inst.i)
    op1 = vm->cpu.inst.imm;
  else
    op1 = vm->cpu.regs[vm->cpu.inst.rb];

  if (vm->cpu.inst.bmode)
  {
    op0 &= 0xff;
    op1 &= 0xff;
  }

  (op0 == op1) ? SET_ZF : CLR_ZF;
  (op0 < op1)  ? SET_LT : CLR_LT;
  (op0 > op1)  ? SET_GT : CLR_GT;
  
}


void op_cmps(vm_t *vm)
{
  int16_t op0, op1;
  int8_t op0b, op1b;

  op0 = (int16_t)vm->cpu.regs[vm->cpu.inst.ra];

  if (vm->cpu.inst.i)
    op1 = (int16_t)vm->cpu.inst.imm;
  else
    op1 = (int16_t)vm->cpu.regs[vm->cpu.inst.rb];

  if (vm->cpu.inst.bmode)
  {
    op0b = (int8_t)op0;
    op1b = (int8_t)op1;
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

}


void op_nop(vm_t *vm)
{
}

void op_br(vm_t *vm)
{
  if (vm->cpu.inst.i)
    vm->cpu.pc += vm->cpu.inst.imm;
  else
    vm->cpu.pc = vm->cpu.regs[vm->cpu.inst.ra];

}

void op_bre(vm_t *vm)
{
  if (ZF)
  {
    if (vm->cpu.inst.i)
      vm->cpu.pc += vm->cpu.inst.imm;
    else
      vm->cpu.pc = vm->cpu.regs[vm->cpu.inst.ra];
  }
}

void op_brne(vm_t *vm)
{
  if (!ZF)
  {
    if (vm->cpu.inst.i)
      vm->cpu.pc += vm->cpu.inst.imm;
    else
      vm->cpu.pc = vm->cpu.regs[vm->cpu.inst.ra];
  }
}

void op_brg(vm_t *vm)
{
  if (GT)
  {
    if (vm->cpu.inst.i)
      vm->cpu.pc += vm->cpu.inst.imm;
    else
      vm->cpu.pc = vm->cpu.regs[vm->cpu.inst.ra];
  }
}

void op_brge(vm_t *vm)
{
  if (GT || ZF)
  {
    if (vm->cpu.inst.i)
      vm->cpu.pc += vm->cpu.inst.imm;
    else
      vm->cpu.pc = vm->cpu.regs[vm->cpu.inst.ra];
  }
}

void op_brl(vm_t *vm)
{
  if (LT)
  {
    if (vm->cpu.inst.i)
      vm->cpu.pc += vm->cpu.inst.imm;
    else
      vm->cpu.pc = vm->cpu.regs[vm->cpu.inst.ra];
  }
}

void op_brle(vm_t *vm)
{
  if (LT || ZF)
  {
    if (vm->cpu.inst.i)
      vm->cpu.pc += vm->cpu.inst.imm;
    else
      vm->cpu.pc = vm->cpu.regs[vm->cpu.inst.ra];
  }
}

void op_call(vm_t *vm)
{
  if (cpu_w_mem(vm, vm->cpu.regs[15] - 2, vm->cpu.pc, REQ_WRITE_W))
  {
    vm->cpu.regs[15] -= 2;
    if (vm->cpu.inst.i)
      vm->cpu.pc += vm->cpu.inst.imm;
    else
      vm->cpu.pc = vm->cpu.regs[vm->cpu.inst.ra];
  }
  else
  {
    fprintf(stderr, "Bus error 4\n");
  }
}

void op_back(vm_t *vm)
{
  uint32_t data;
  if (cpu_r_mem(vm, vm->cpu.regs[15], &data))
  {
    vm->cpu.pc = data & 0xffff;
    vm->cpu.regs[15] += 2;
  }
  else
  {
    fprintf(stderr, "Bus error 5\n");
  }
}

void op_ldflg(vm_t *vm)
{
  vm->cpu.regs[0] = vm->cpu.flags;
}

void op_stflg(vm_t *vm)
{
  vm->cpu.flags = vm->cpu.regs[0];
}

void op_ldccr(vm_t *vm)
{
  vm->cpu.regs[0] = vm->cpu.ccr;
}

void op_stccr(vm_t *vm)
{
  vm->cpu.ccr = vm->cpu.regs[0];
}

void op_ldicr(vm_t *vm)
{
  vm->cpu.regs[0] = vm->cpu.icr;
}

void op_sticr(vm_t *vm)
{
  vm->cpu.icr = vm->cpu.regs[0];
}

void op_ldtcr(vm_t *vm)
{
  vm->cpu.regs[0] = vm->cpu.tcr;
}

void op_sttcr(vm_t *vm)
{
  vm->cpu.tcr = vm->cpu.regs[0];
}

void op_ldacr(vm_t *vm)
{
  vm->cpu.regs[0] = vm->cpu.acr;
}

void op_stacr(vm_t *vm)
{
  vm->cpu.acr = vm->cpu.regs[0];
}

void op_ldctx(vm_t *vm)
{

  unsigned short i, addr = vm->cpu.regs[0];
  uint32_t data;

  for (i = 0; i < ICACHE_SIZE; i++)
    vm->cpu.icache_addr[i] = -1;

  cpu_r_mem(vm, addr + 2, &data);
  vm->cpu.flags = data & 0xffff;

  cpu_r_mem(vm, addr + 4, &data);
  vm->cpu.acr = data & 0xffff;

  cpu_r_mem(vm, addr + 6, &data);
  vm->cpu.pc = data & 0xffff;

  for (i = 0; i < 16; i++)
  {
    cpu_r_mem(vm, addr + 8 + (i*2), &data);
    vm->cpu.regs[i] = data & 0xffff;
  }

  ENTER_SUPERVISOR_MODE;
  vm->cpu.time_slice = QUANTUM;
}

void op_stctx(vm_t *vm)
{
  uint16_t i;

  cpu_w_mem(vm, vm->cpu.regs[0] + 2, vm->cpu._flags, REQ_WRITE_W);
  cpu_w_mem(vm, vm->cpu.regs[0] + 4, vm->cpu.acr, REQ_WRITE_W);
  cpu_w_mem(vm, vm->cpu.regs[0] + 6, vm->cpu._pc, REQ_WRITE_W);
  for (i = 0; i < 16; i++)
    cpu_w_mem(vm, vm->cpu.regs[0] + 8+(i*2), vm->cpu._regs[i], REQ_WRITE_W);

}


void op_di(vm_t *vm)
{
  vm->cpu.ccr = vm->cpu.ccr & 0xfffe;
}

void op_ei(vm_t *vm)
{
  vm->cpu.ccr = vm->cpu.ccr | 0x0001;
  vm->cpu.time_slice = QUANTUM;
}

static void alu(vm_t *vm, uint32_t operands)
{
  uint16_t op0, op1, res;
  uint16_t sign0, sign1, sign2, cf;


  if (operands == 2)
  {
    op0 = vm->cpu.regs[vm->cpu.inst.rb];

    if (vm->cpu.inst.i)
      op1 = vm->cpu.inst.imm;
    else
      op1 = vm->cpu.regs[vm->cpu.inst.rc];

    if (vm->cpu.inst.bmode)
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
  
    switch(vm->cpu.inst.op)
    {
      case ADD:
      {
        res = op0 + op1;
        sign2 = (vm->cpu.inst.bmode) ? (res & 0x80) : (res & 0x8000);
        ((sign0 == sign1) && (sign0 != sign2)) ? SET_OF : CLR_OF;
      }
      break;

      case SUB:
      {
        res = op0 - op1;
        sign2 = (vm->cpu.inst.bmode) ? (res & 0x80) : (res & 0x8000);
        ((sign0 == sign1) && (sign0 != sign2)) ? SET_OF : CLR_OF;
      } 
      break;

      case DIV:
      {
        if (op1 != 0)
        {
          res = op0 / op1;
          sign2 = (vm->cpu.inst.bmode) ? (res & 0x80) : (res & 0x8000);
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
        sign2 = (vm->cpu.inst.bmode) ? (res & 0x80) : (res & 0x8000);
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
          cf = (vm->cpu.inst.bmode) ? (op0 & 0x80) : (op0 & 0x8000);
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
    res = vm->cpu.regs[vm->cpu.inst.ra];
    if (vm->cpu.inst.bmode)
      res = (res & 0xff00) | (res & 0xff);

    switch(vm->cpu.inst.op)
    {
      case INC: res++; break;
      case DEC: res--; break;
      case NOT: res = ~res; break;
      default: break;
    }
  }

  if (vm->cpu.inst.bmode)
  {
    (res & 0x80) ? SET_SF : CLR_SF;
    res = (vm->cpu.regs[vm->cpu.inst.ra] & 0xff00) | (res & 0xff);
  }
  else
  {
    (res & 0x8000) ? SET_SF : CLR_SF;
  }
  
  (res == 0) ? SET_ZF : CLR_ZF;

  vm->cpu.regs[vm->cpu.inst.ra] = res;
}


int cpu_w_mem(vm_t *vm, uint16_t mar,
                  uint32_t mdr, uint32_t m)
{
  uint32_t retval = 0;

  pthread_mutex_lock(&vm->mmu.lock);
  vm->mmu.mdr = mdr;
  vm->mmu.mar = mar;
  vm->mmu.control = m;
  pthread_mutex_unlock(&vm->mmu.lock);

  usleep(2000);

  pthread_mutex_lock(&vm->mmu.lock);
  if (vm->mmu.control == RES_WRITE_OK)
    retval = 1;
  pthread_mutex_unlock(&vm->mmu.lock);
  return retval;
}


int cpu_r_mem(vm_t *vm, uint16_t mar, uint32_t *data)
{
  uint32_t retval = 0;

  pthread_mutex_lock(&vm->mmu.lock);
  vm->mmu.mar = mar;
  vm->mmu.control = REQ_READ;
  pthread_mutex_unlock(&vm->mmu.lock);

  usleep(2000);

  pthread_mutex_lock(&vm->mmu.lock);
  if (vm->mmu.control == RES_READ_OK)
  {
    *data = vm->mmu.mdr;
    retval = 1;
  }
  pthread_mutex_unlock(&vm->mmu.lock);
  return retval;
}