#include "vm.h"
#include "isa.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


static int init_io_bus(vm_t *vm);
static int init_mem_bus(vm_t *vm);
static int init_cpu(vm_t *vm);
static int init_ram(vm_t *vm);
static void load_code(vm_t *vm, char *code_file);
static off_t getfsize(const char *filename);


int turn_on(vm_t *vm, char *code_file)
{

  pthread_t cpu_thread, mmu_thread;//, io_thread;

  printf("Turning on...\n");
  if (!init_io_bus(vm) || !init_mem_bus(vm) || !init_cpu(vm) || !init_ram(vm))
  {
    fprintf(stderr, "VM Fatal Err\n");
    turn_off(vm);
    exit(EXIT_FAILURE);
  }
  sleep(2);

  load_code(vm, code_file);
  vm->debug_mode = 1;

  if (pthread_create(&cpu_thread, NULL, cpu_uc,  (void *)vm) != 0)
  {
    fprintf(stderr, "Failed to create cpu_thread\n");
    turn_off(vm);
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&mmu_thread, NULL, mmu_service, (void *)vm) != 0)
  {
    fprintf(stderr, "Failed to create mmu_thread\n");
    turn_off(vm);
    exit(EXIT_FAILURE);
  }

  if (pthread_mutex_init(&vm->mem_bus->lock, NULL) != 0)
  {
    fprintf(stderr, "Mutex init failed\n");
    turn_off(vm);
    exit(EXIT_FAILURE);
  }

  if (pthread_mutex_init(&vm->io_bus->lock, NULL) != 0)
  {
    fprintf(stderr, "Mutex init failed\n");
    turn_off(vm);
    exit(EXIT_FAILURE);
  }


  pthread_join(cpu_thread, NULL);
  pthread_join(mmu_thread, NULL);

  pthread_mutex_destroy(&vm->mem_bus->lock);
  pthread_mutex_destroy(&vm->io_bus->lock);

  return 1;
}

static void load_code(vm_t *vm, char *code_file)
{
  FILE *in;

  unsigned int size = getfsize(code_file);
  if (size >= RAM_SIZE)
  {
    fprintf(stderr, "code too big to fit in memory\n");
    turn_off(vm);
  }

  in = fopen(code_file, "rb");
  if (!in)
  {
    perror("");
    turn_off(vm);
  }
  fread(vm->ram, sizeof(unsigned char), size, in);
  fclose(in);

}

static off_t getfsize(const char *filename)
{
  struct stat st;
  stat(filename, &st);
  return st.st_size;
}


int turn_off(vm_t *vm)
{
  printf("Turning off...\n");
  
  if (vm->io_bus)
    free(vm->io_bus);

  if (vm->mem_bus)
    free(vm->mem_bus);

  if (vm->cpu)
  {
    if (vm->cpu->inst)
      free(vm->cpu->inst);
    
    free(vm->cpu);
  }

  if (vm->ram)
    free(vm->ram);

  free(vm);

  return 1;
}

vm_t *create_vm(void)
{
  vm_t *vm;
  vm = (vm_t *)malloc(sizeof(vm_t));
  
  if(!vm)
  {
    perror("vm");
    exit(EXIT_FAILURE);
  }

  vm->io_bus = NULL;
  vm->mem_bus = NULL;
  vm->cpu = NULL;
  vm->ram = NULL;

  return vm;  
}

static int init_io_bus(vm_t *vm)
{
  vm->io_bus = (io_bus_t *)malloc(sizeof(io_bus_t));
  memset(vm->io_bus, 0, sizeof(io_bus_t));
  printf("  I/O BUS - %s\n", (vm->io_bus) ? "OK" : "ERR");
  return (vm->io_bus) ? 1 : 0;
}

static int init_mem_bus(vm_t *vm)
{
  vm->mem_bus = (mem_bus_t *)malloc(sizeof(mem_bus_t));
  memset(vm->mem_bus, 0, sizeof(mem_bus_t));
  printf("  MEM BUS - %s\n", (vm->mem_bus) ? "OK" : "ERR");
  return (vm->mem_bus) ? 1 : 0;
}

static int init_cpu(vm_t *vm)
{
  vm->cpu = (cpu_t *)malloc(sizeof(cpu_t));
  memset(vm->cpu, 0, sizeof(cpu_t));
  
  if (vm->cpu)  
    vm->cpu->inst = (inst_t *)malloc(sizeof(inst_t));

  memset(vm->cpu->regs, 0, sizeof(vm->cpu->regs));
  memset(vm->cpu->isa, 0, sizeof(vm->cpu->isa));

  ENABLE_INTERRUPTION;

  vm->cpu->isa[MOV] = isa_mov;
  vm->cpu->isa[EXT] = isa_ext;
  vm->cpu->isa[EXTS] = isa_exts;

  vm->cpu->isa[ADD] = isa_add;
  vm->cpu->isa[SUB] = isa_sub;
  vm->cpu->isa[MUL] = isa_mul;
  vm->cpu->isa[DIV] = isa_div;
  vm->cpu->isa[INC] = isa_inc;
  vm->cpu->isa[DEC] = isa_dec;

  vm->cpu->isa[OR] = isa_or;
  vm->cpu->isa[AND] = isa_and;
  vm->cpu->isa[XOR] = isa_xor;
  vm->cpu->isa[SHL] = isa_shl;
  vm->cpu->isa[SHR] = isa_shr;
  vm->cpu->isa[NOT] = isa_not;

  vm->cpu->isa[CMP] = isa_cmp;
  vm->cpu->isa[CMPS] = isa_cmps;

  vm->cpu->isa[HLT] = isa_hlt;
  vm->cpu->isa[NOP] = isa_nop;
  vm->cpu->isa[PSH] = isa_psh;
  vm->cpu->isa[POP] = isa_pop;

  vm->cpu->isa[BR] = isa_br;
  vm->cpu->isa[BRE] = isa_bre;
  vm->cpu->isa[BRNE] = isa_brne;
  vm->cpu->isa[BRG] = isa_brg;
  vm->cpu->isa[BRGE] = isa_brge;
  vm->cpu->isa[BRL] = isa_brl;
  vm->cpu->isa[BRLE] = isa_brl;

  vm->cpu->isa[LOAD] = isa_load;
  vm->cpu->isa[STORE] = isa_store;

  vm->cpu->isa[CALL] = isa_call;
  vm->cpu->isa[BACK] = isa_back;

  vm->cpu->isa[LDFLG] = isa_ldflg;
  vm->cpu->isa[STFLG] = isa_stflg;
  vm->cpu->isa[LDCCR] = isa_ldccr;
  vm->cpu->isa[STCCR] = isa_stccr;
  vm->cpu->isa[LDICR] = isa_ldicr;
  vm->cpu->isa[STICR] = isa_sticr;
  vm->cpu->isa[LDTCR] = isa_ldtcr;
  vm->cpu->isa[STTCR] = isa_sttcr;

  vm->cpu->isa[LDCTX] = isa_ldctx;
  vm->cpu->isa[STCTX] = isa_stctx;

  vm->cpu->isa[DI] = isa_di;
  vm->cpu->isa[EI] = isa_ei;
  vm->cpu->isa[IBACK] = isa_iback;

  vm->cpu->isa[PSHA] = isa_psha;
  vm->cpu->isa[POPA] = isa_popa;

  memset(vm->cpu->icache_addr, 0xff, sizeof(vm->cpu->icache_addr));
  vm->cpu->icache_oldest = 0;

/*
  // Test
  int i;
  for (i = 0; i < 16; i++)
    vm->cpu->regs[i] = i;
*/


  printf("  CPU - %s\n", (vm->cpu && vm->cpu->inst) ? "OK" : "ERR");
  return (vm->cpu && vm->cpu->inst) ? 1 : 0;
}

static int init_ram(vm_t *vm)
{
  unsigned int size = (unsigned int)sizeof(ram_t) * RAM_SIZE;
  vm->ram = (ram_t *)malloc(size);
  memset(vm->ram, 0, size);
  printf("  RAM (0x%x bytes) - %s\n", size, (vm->ram) ? "OK" : "ERR");
  return (vm->ram) ? 1 : 0;
}