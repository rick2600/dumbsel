#include "vm.h"
#include "cpu.h"
#include "ram.h"
#include "isa.h"
#include <stdio.h>
#include <stdlib.h>


static void show_registers(cpu_t *cpu);
static void show_memory(ram_t *ram);
static void show_instruction(cpu_t *cpu);
static void init_names(void);
static void show_memory_and_disas(vm_t *vm);
static void show_stack(vm_t *vm);
static void disas(unsigned int raw_inst);



int showpc, ok2inc;

int init;
char *inst_names[64];

void run_debugger(vm_t *vm, int r, int m, int i, int stop)
{
  showpc = 0;
  ok2inc = 0;
  if (!init)
    init_names();

  printf("\n\n");
  if (r)
    show_registers(vm->cpu);

  if (m)
    show_memory(vm->ram);

  if (i)
  show_instruction(vm->cpu);

  if(stop)
  {
    printf("-> ");
    getchar();
  }
}

void run_debugger2(vm_t *vm, int stop)
{
  int a;

  if (!init)
    init_names();

  system("/usr/bin/clear");
  printf("Dumbsel Debugger:\n\n");

  show_registers(vm->cpu);

  for (a=0;a < 8; a++)
    printf("=============");
  printf("\n");

  show_memory_and_disas(vm);

  for (a=0;a < 8; a++)
    printf("=============");
  printf("\n");

  show_stack(vm);

  if(stop)
  {
    printf("-> ");
    getchar();
  }
}

static void show_stack(vm_t *vm)
{
  int i;
  unsigned short int v;
  for (i = 0; i < 16; i+=2)
  {
    v = *(unsigned short int *)&vm->ram[vm->cpu->regs[15]+i];
    v &= 0xffff;
    printf("ts+%02x: |0x%04x| 0x%04x\n", i, vm->cpu->regs[15]+i, v);
  }
}


static void show_memory_and_disas(vm_t *vm)
{
  int i, j, pos = 0;
  int inst, temp;
  //for (i = 0; i < RAM_SIZE; i+=16)
  for (i = 0; i < 256; i+=16)
  {
    printf("0x%04x: ", i);
    for (j = 0; j < 16; j++)
      printf("%02x ", vm->ram[j+i]);

    temp = showpc+(pos*4);
    inst = SWAP_UINT32(*(unsigned int *)&vm->ram[temp]);
    printf(" |%s0x%04x | ", (temp == (vm->cpu->pc-4)) ? " pc: ": "     ", temp);
    
    disas(inst);
    pos++;
    printf("\n");
  }
  if (vm->cpu->pc == 32)
    ok2inc = 1;

  if(ok2inc)
    showpc += 4;
}

static void disas(unsigned int raw_inst)
{
  unsigned int op = raw_inst >> 26;
  unsigned char has_imm = (raw_inst >> 25) & 1;
  unsigned char byte_mode = (raw_inst >> 24) & 1;
  unsigned char ra = (raw_inst >> 20) & 0x0f;
  unsigned char rb = (raw_inst >> 16) & 0x0f;
  unsigned char rc = (raw_inst >> 12) & 0x0f;
  unsigned short int imm = SWAP_UINT16(raw_inst & 0xffff);
  

  char oper[16];
  char *regs[] = {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8",
    "r9", "r10", "r11", "r12", "r13", "bs", "ts"
  };

  printf("%08x    ", raw_inst);
  switch(op)
  {

    case HLT:
    {
      printf("%s", inst_names[HLT]);
    }
    break;

    case MOV:
    case EXT:
    case EXTS:
    {
      if (has_imm)
        sprintf(oper, "0x%04x", imm);
      else
        sprintf(oper, "%s", regs[rb]);

      printf("%s%s %s,%s", 
        inst_names[op], 
        (byte_mode) ? "b":"",
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
      if (has_imm)
        sprintf(oper, "0x%04x", imm);
      else
        sprintf(oper, "%s", regs[rc]);

      printf("%s%s %s,%s,%s", 
        inst_names[op],
        (byte_mode) ? "b":"",
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
        (byte_mode) ? "b":"",
        regs[ra]
      );
    }
    break;

    default: printf("?"); break;
  }
}

static void show_registers(cpu_t *cpu)
{
  int i;
  //printf("Registers:\n");
  for (i = 0; i < 8; i++)
    printf("r%02d: 0x%04x  ", i, cpu->regs[i]);
  printf("\n");

  for (i = 8; i < 14; i++)
    printf("r%02d: 0x%04x  ", i, cpu->regs[i]);
  printf("bs:  0x%04x  ts:  0x%04x  \n", cpu->regs[14], cpu->regs[15]);

  printf("pc:  0x%04x\n", cpu->pc - 4);
}

static void show_memory(ram_t *ram)
{
  int i, j;
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

static void show_instruction(cpu_t *cpu)
{
  char oper[16];
  char *regs[] = {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8",
    "r9", "r10", "r11", "r12", "r13", "bs", "ts"
  };

  printf("Instruction:\n");
  printf("0x%04x -> ", cpu->pc - 4);
  switch(cpu->inst->op)
  {

    case HLT:
    {
      printf("%s\n", inst_names[HLT]);
    }
    break;

    case MOV:
    case EXT:
    case EXTS:
    {
      if (cpu->inst->has_imm)
        sprintf(oper, "0x%04x", cpu->inst->imm);
      else
        sprintf(oper, "%s", regs[cpu->inst->rb]);

      printf("%s%s %s,%s\n", 
        inst_names[cpu->inst->op], 
        (cpu->inst->byte_mode) ? "b":"",
        regs[cpu->inst->ra],
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
      if (cpu->inst->has_imm)
        sprintf(oper, "0x%04x", cpu->inst->imm);
      else
        sprintf(oper, "%s", regs[cpu->inst->rc]);

      printf("%s%s %s,%s,%s\n", 
        inst_names[cpu->inst->op],
        (cpu->inst->byte_mode) ? "b":"",
        regs[cpu->inst->ra],
        regs[cpu->inst->rb],
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
        inst_names[cpu->inst->op], 
        (cpu->inst->byte_mode) ? "b":"",
        regs[cpu->inst->ra]
      );
    }
    break;

    default: printf("inv (%02x) (0x%08x)\n", cpu->inst->op, cpu->ir); break;
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
  inst_names[SHR] = "not";
  inst_names[HLT] = "hlt";
  inst_names[POP] = "pop";
  inst_names[LOAD] = "load";
  inst_names[STORE] = "store";
}
