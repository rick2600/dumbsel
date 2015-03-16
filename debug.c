#include "vm.h"
#include "cpu.h"
#include "ram.h"
#include "mmu.h"
#include "isa.h"
#include <stdio.h>
#include <stdlib.h>


static void show_registers(vm_t *vm);
static void show_memory(ram_t *ram);
static void show_instruction(vm_t *vm);
static void init_names(void);
static void show_memory_and_disas(vm_t *vm);
static void show_stack(vm_t *vm);
static void disas(uint16_t addr, uint32_t raw_inst);
//static void show_stack_and_registers(vm_t *vm);



int showpc, ok2inc;

int init;
char *inst_names[64];

void run_debugger(vm_t *vm, uint32_t r, uint32_t m, uint32_t i, uint32_t stop)
{
  showpc = 0;
  ok2inc = 0;
  if (!init)
    init_names();

  printf("\n\n");
  if (r)
    show_registers(vm);

  if (m)
    show_memory(vm->ram);

  if (i)
  show_instruction(vm);

  if(stop)
  {
    printf("-> ");
    getchar();
  }
}

void run_debugger2(vm_t *vm, uint32_t stop)
{
  uint32_t a;

  if (!init)
    init_names();

  //system("/usr/bin/clear");
  printf("Dumbsel Debugger:\n\n");

  show_registers(vm);

  for (a=0;a < 8; a++)
    printf("=============");
  printf("\n");

  show_memory_and_disas(vm);

  for (a=0;a < 8; a++)
    printf("=============");
  printf("\n");

  show_stack(vm);
  //show_stack_and_registers(vm);

  if(stop)
  {
    printf("-> ");
    getchar();
  }
}

/*
static void show_stack_and_registers(vm_t *vm)
{
  uint32_t i, j;
  char *regs[] = {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8 ",
    "r9 ", "r10", "r11", "r12", "r13", "bs ", "ts "
  };
  char buf[128];
  uint16_t v;
  for (i = 0, j = 0; i < 16; i+=2, j++)
  {
    v = *(uint16_t *)&vm->ram[vm->cpu.regs[15]+i];
    v &= 0xffff;

    sprintf(buf, "%s: 0x%04x (u: %5hu, s: %5hd)  .  %s: 0x%04x (u: %5hu, s: %5hd)", 
      regs[j], vm->cpu.regs[j], vm->cpu.regs[j], (signed int)vm->cpu.regs[j],
      regs[j+8], vm->cpu.regs[j+8], vm->cpu.regs[j+8], (signed int)vm->cpu.regs[j+8]);


    printf("ts+%02x: |0x%04x| 0x%04x |   %s\n", i, vm->cpu.regs[15]+i, v, buf);
  }
}
*/

/*
static void show_stack(vm_t *vm)
{
  uint32_t i, r, w;
  uint16_t v, addr;
  //addr = mmu_translation_unit(vm, &r, &w);
  for (i = 0; i < 20; i+=2)
  {
    v = *(uint16_t *)&vm->ram[addr+i];
    v &= 0xffff;
    printf("ts+%02x: |0x%04x| 0x%04x\n", i, addr+i, v);
  }
}
*/

static void show_stack(vm_t *vm)
{
  uint32_t i;
  uint16_t v;
  for (i = 0; i < 20; i+=2)
  {
    v = *(uint16_t *)&vm->ram[vm->cpu.regs[15]+i];
    v &= 0xffff;
    printf("ts+%02x: |0x%04x| 0x%04x\n", i, vm->cpu.regs[15]+i, v);
  }
}

/*
static void show_memory_and_disas(vm_t *vm)
{
  uint32_t i, j, pos = 0;
  uint32_t inst, temp;
  //for (i = 0; i < RAM_SIZE; i+=16)
  for (i = 0; i < 256; i+=16)
  {
    printf("0x%04x: ", i);
    for (j = 0; j < 16; j++)
      printf("%02x ", vm->ram[j+i]);

    temp = showpc+(pos*4);
    inst = SWAP_UINT32(*(uint32_t *)&vm->ram[temp]);
    printf(" |%s0x%04x | ", (temp == (vm->cpu.pc-4)) ? " pc: ": "     ", temp);
    
    disas(inst);
    pos++;
    printf("\n");
  }
  if (vm->cpu.pc == 32)
    ok2inc = 1;

  if(ok2inc)
    showpc += 4;
}
*/

static void show_memory_and_disas(vm_t *vm)
{
  int32_t i, j, pos = vm->cpu.pc - 4;
  uint32_t inst;
  //for (i = 0; i < RAM_SIZE; i+=16)
  for (i = 0; i < 256; i+=16)
  {
    printf("0x%04x: ", i);
    for (j = 0; j < 16; j++)
      printf("%02x ", vm->ram[j+i]);

    inst = SWAP_UINT32(*(uint32_t *)&vm->ram[pos]);
    printf(" |%s0x%04x | ", (pos == (vm->cpu.pc-4)) ? " pc: ": "     ", pos);
    
    disas(pos+4, inst);
    pos += 4;
    printf("\n");
  }
  /*
  if (!(vm->cpu.pc % 32))
    ok2inc = 1;

  if(ok2inc)
    showpc += 4;
  */
}

static void disas(uint16_t addr, uint32_t raw_inst)
{
  uint32_t op = raw_inst >> 26;
  uint8_t i = (raw_inst >> 25) & 1;
  uint8_t bmode = (raw_inst >> 24) & 1;
  uint8_t ra = (raw_inst >> 20) & 0x0f;
  uint8_t rb = (raw_inst >> 16) & 0x0f;
  uint8_t rc = (raw_inst >> 12) & 0x0f;
  uint16_t imm = SWAP_UINT16(raw_inst & 0xffff);
  

  char oper[16];
  char *regs[] = {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8",
    "r9", "r10", "r11", "r12", "r13", "bs", "ts"
  };

  printf("%08x    ", raw_inst);
  switch(op)
  {

    case PSH:
    case BR:
    case BRE:
    case BRNE:
    case BRG:
    case BRGE:
    case BRL:
    case BRLE:
    case CALL:
    {
      if (i)
      {
        if (op != PSH)
          sprintf(oper, "<0x%04x>", (addr + imm) & 0xfff);
        else
          sprintf(oper, "0x%04x", imm);
      }
      else
        sprintf(oper, "%s", regs[ra]);


      printf("%s %s", inst_names[op], oper);
    }
    break;

    case LDACR:
    case STACR:
    case LDICR:
    case STICR:
    case LDTCR:
    case STTCR:
    case DI:
    case EI:
    case LDCCR:
    case STCCR:
    case LDCTX:
    case STCTX:
    case LDFLG:
    case STFLG:
    case BACK:
    case NOP:
    case HLT:
    case IBACK:
    case PSHA:
    case POPA:
    {
      printf("%s", inst_names[op]);
    }
    break;

    case MOV:
    case EXT:
    case EXTS:
    case CMP:
    case CMPS:
    {
      if (i)
        sprintf(oper, "0x%04x", imm);
      else
        sprintf(oper, "%s", regs[rb]);

      printf("%s%s %s,%s", 
        inst_names[op], 
        (bmode) ? "b":"",
        regs[ra],
        oper
      );
    }
    break;

    case ADD:
    case SUB:
    case MUL:
    case DIV:
    case OR:
    case AND:
    case XOR:
    case SHL:
    case SHR:
    case LOAD:
    case STORE:
    {
      if (i)
        sprintf(oper, "0x%04x", imm);
      else
        sprintf(oper, "%s", regs[rc]);

      printf("%s%s %s,%s,%s", 
        inst_names[op],
        (bmode) ? "b":"",
        regs[ra],
        regs[rb],
        oper
      );
    }
    break;

    case INC:
    case DEC:
    case NOT:
    case POP:
    {
      printf("%s%s %s",
        inst_names[op], 
        (bmode) ? "b":"",
        regs[ra]
      );
    }
    break;

    default: printf("?"); break;
  }
}
/*
static void show_registers(cpu_t *cpu)
{
  uint32_t i;
  //printf("Registers:\n");
  for (i = 0; i < 8; i++)
    printf("r%02d: 0x%04x  ", i, cpu.regs[i]);
  printf("\n");

  for (i = 8; i < 14; i++)
    printf("r%02d: 0x%04x  ", i, cpu.regs[i]);
  printf("bs:  0x%04x  ts:  0x%04x  \n", cpu.regs[14], cpu.regs[15]);

  printf("pc:  0x%04x  flags: 0x%04x [", cpu.pc - 4, cpu.flags);
  printf(" %s%s%s]\n", 
    ZF(cpu.flags) ? "ZF ": "",
    LT(cpu.flags) ? "LT ": "",
    GT(cpu.flags) ? "GT ": ""
  );
}
*/
static void show_registers(vm_t *vm)
{
  uint32_t i;
  char *regs[] = {
    "r0 ", "r1 ", "r2 ", "r3 ", "r4 ", "r5 ", "r6 ", "r7 ", "r8 ",
    "r9 ", "r10", "r11", "r12", "r13", "bs ", "ts "
  };

  for (i = 0; i < 8; i++)
    printf("%s: 0x%04x (u: %5hu, s: %5hd)        %s: 0x%04x (u: %5hu, s: %5hd)\n", 
      regs[i], vm->cpu.regs[i], vm->cpu.regs[i], (signed int)vm->cpu.regs[i],
      regs[i+8], vm->cpu.regs[i+8], vm->cpu.regs[i+8], (signed int)vm->cpu.regs[i+8]);
  printf("\n");
  printf("pc:  0x%04x    ccr: 0x%04x    "\
    "icr: 0x%04x    tcr: 0x%04x    acr: 0x%04x    t: %d    flags: 0x%04x [", 
      vm->cpu.pc - 4, vm->cpu.ccr, vm->cpu.icr, vm->cpu.tcr, 
      vm->cpu.acr, vm->cpu.time_slice, vm->cpu.flags);
  printf(" %s%s%s%s%s%s]\n", 
    ZF ? "ZF ": "",
    LT ? "LT ": "",
    GT ? "GT ": "",
    CF ? "CF ": "",
    SF ? "SF ": "",
    OF ? "OF ": ""
  );

}

static void show_memory(ram_t *ram)
{
  uint32_t i, j;
  printf("Memory:\n");
  for (i = 0; i < RAM_SIZE; i+=16)
  {
    printf("0x%04x: ", i);
    for (j = 0; j < 16; j++)
      printf("%02x ", ram[j+i]);
    printf("\n");
  }
  printf("\n\n");
}

static void show_instruction(vm_t *vm)
{
  char oper[16];
  char *regs[] = {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8",
    "r9", "r10", "r11", "r12", "r13", "bs", "ts"
  };

  printf("Instruction:\n");
  printf("0x%04x -> ", vm->cpu.pc - 4);
  switch(vm->cpu.inst.op)
  {

    case PSH:
    case BR:
    case BRE:
    case BRNE:
    case BRG:
    case BRGE:
    case BRL:
    case BRLE:
    {
      if (vm->cpu.inst.i)
        sprintf(oper, "0x%04x", vm->cpu.inst.imm);
      else
        sprintf(oper, "%s", regs[vm->cpu.inst.rb]);
      printf("%s %s", inst_names[vm->cpu.inst.op], oper);
    }
    break;

    case HLT:
    case NOP:
    {
      printf("%s\n", inst_names[vm->cpu.inst.op]);
    }
    break;

    case MOV:
    case EXT:
    case EXTS:
    {
      if (vm->cpu.inst.i)
        sprintf(oper, "0x%04x", vm->cpu.inst.imm);
      else
        sprintf(oper, "%s", regs[vm->cpu.inst.rb]);

      printf("%s%s %s,%s\n", 
        inst_names[vm->cpu.inst.op], 
        (vm->cpu.inst.bmode) ? "b":"",
        regs[vm->cpu.inst.ra],
        oper
      );
    }
    break;

    case ADD:
    case SUB:
    case MUL:
    case DIV:
    case OR:
    case AND:
    case XOR:
    case SHL:
    case SHR:
    case LOAD:
    case STORE:
    {
      if (vm->cpu.inst.i)
        sprintf(oper, "0x%04x", vm->cpu.inst.imm);
      else
        sprintf(oper, "%s", regs[vm->cpu.inst.rc]);

      printf("%s%s %s,%s,%s\n", 
        inst_names[vm->cpu.inst.op],
        (vm->cpu.inst.bmode) ? "b":"",
        regs[vm->cpu.inst.ra],
        regs[vm->cpu.inst.rb],
        oper
      );
    }
    break;

    case INC:
    case DEC:
    case NOT:
    case POP:
    {
      printf("%s%s %s\n",
        inst_names[vm->cpu.inst.op], 
        (vm->cpu.inst.bmode) ? "b":"",
        regs[vm->cpu.inst.ra]
      );
    }
    break;

    default: printf("inv (%02x) (0x%08x)\n", vm->cpu.inst.op, vm->cpu.ir); break;
  }
  
}

static void init_names(void)
{
  init = 1;
  inst_names[MOV] = "mov";
  inst_names[EXT] = "ext";
  inst_names[EXTS] = "exts";
  inst_names[ADD] = "add";
  inst_names[SUB] = "sub";
  inst_names[MUL] = "mul";
  inst_names[DIV] = "div";
  inst_names[INC] = "inc";
  inst_names[DEC] = "dec";
  inst_names[NOT] = "not";
  inst_names[OR] = "or";
  inst_names[XOR] = "xor";
  inst_names[AND] = "and";
  inst_names[SHL] = "shl";
  inst_names[SHR] = "shr";
  inst_names[HLT] = "hlt";
  inst_names[PSH] = "psh";
  inst_names[POP] = "pop";
  inst_names[LOAD] = "load";
  inst_names[STORE] = "store";
  inst_names[CMP] = "cmp";
  inst_names[CMPS] = "cmps";
  inst_names[NOP] = "nop";
  inst_names[BR] = "br";
  inst_names[BRE] = "bre";
  inst_names[BRNE] = "brne";
  inst_names[BRG] = "brg";
  inst_names[BRGE] = "brge";
  inst_names[BRL] = "brl";
  inst_names[BRLE] = "brle";
  inst_names[CALL] = "call";
  inst_names[BACK] = "back";
  inst_names[LDFLG] = "ldflg";
  inst_names[STFLG] = "stflg";
  inst_names[LDCCR] = "ldccr";
  inst_names[STCCR] = "stccr";
  inst_names[LDICR] = "ldicr";
  inst_names[STICR] = "sticr";
  inst_names[LDTCR] = "ldtcr";
  inst_names[STTCR] = "sttcr";
  inst_names[LDACR] = "ldacr";
  inst_names[STACR] = "stacr";
  inst_names[DI] = "di";
  inst_names[EI] = "ei";
  inst_names[IBACK] = "iback";
  inst_names[PSHA] = "psha";
  inst_names[POPA] = "popa";
  inst_names[LDCTX] = "ldctx";
  inst_names[STCTX] = "stctx";

}

