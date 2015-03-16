#include "vm.h"
#include "isa.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


static uint32_t init_io_bus(vm_t *vm);
static uint32_t init_mmu(vm_t *vm);
static uint32_t init_cpu(vm_t *vm);
static uint32_t init_ram(vm_t *vm);
static void load_code(vm_t *vm, char *code_file);
static off_t getfsize(const char *filename);


int turn_on(vm_t *vm, char *code_file)
{

  pthread_t cpu_thread, mmu_thread;//, io_thread;

  printf("Turning on...\n");
  if (!init_io_bus(vm) || !init_mmu(vm) || !init_cpu(vm) || !init_ram(vm))
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

  if (pthread_mutex_init(&vm->mmu.lock, NULL) != 0)
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

  pthread_mutex_destroy(&vm->mmu.lock);
  pthread_mutex_destroy(&vm->io_bus->lock);

  return 1;
}

static void load_code(vm_t *vm, char *code_file)
{
  FILE *in;

  uint32_t size = getfsize(code_file);
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
  fread(vm->ram, sizeof(uint8_t), size, in);
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
  vm->ram = NULL;

  return vm;  
}

static uint32_t init_io_bus(vm_t *vm)
{
  vm->io_bus = (io_bus_t *)malloc(sizeof(io_bus_t));
  memset(vm->io_bus, 0, sizeof(io_bus_t));
  printf("  I/O BUS - %s\n", (vm->io_bus) ? "OK" : "ERR");
  return (vm->io_bus) ? 1 : 0;
}

static uint32_t init_mmu(vm_t *vm)
{  
  memset(&vm->mmu, 0, sizeof(mmu_t));
  printf("  MEM BUS - OK\n");
  return 1;
}

static uint32_t init_cpu(vm_t *vm)
{
  memset(&vm->cpu, 0, sizeof(cpu_t));
  
  //ENABLE_INTERRUPTION;
  ENTER_SUPERVISOR_MODE;
  vm->cpu.time_slice = QUANTUM;


  vm->cpu.opers[MOV] = op_mov;
  vm->cpu.opers[EXT] = op_ext;
  vm->cpu.opers[EXTS] = op_exts;

  vm->cpu.opers[ADD] = op_add;
  vm->cpu.opers[SUB] = op_sub;
  vm->cpu.opers[MUL] = op_mul;
  vm->cpu.opers[DIV] = op_div;
  vm->cpu.opers[INC] = op_inc;
  vm->cpu.opers[DEC] = op_dec;

  vm->cpu.opers[OR] = op_or;
  vm->cpu.opers[AND] = op_and;
  vm->cpu.opers[XOR] = op_xor;
  vm->cpu.opers[SHL] = op_shl;
  vm->cpu.opers[SHR] = op_shr;
  vm->cpu.opers[NOT] = op_not;

  vm->cpu.opers[CMP] = op_cmp;
  vm->cpu.opers[CMPS] = op_cmps;

  vm->cpu.opers[HLT] = op_hlt;
  vm->cpu.opers[NOP] = op_nop;
  vm->cpu.opers[PSH] = op_psh;
  vm->cpu.opers[POP] = op_pop;

  vm->cpu.opers[BR] = op_br;
  vm->cpu.opers[BRE] = op_bre;
  vm->cpu.opers[BRNE] = op_brne;
  vm->cpu.opers[BRG] = op_brg;
  vm->cpu.opers[BRGE] = op_brge;
  vm->cpu.opers[BRL] = op_brl;
  vm->cpu.opers[BRLE] = op_brl;

  vm->cpu.opers[LOAD] = op_load;
  vm->cpu.opers[STORE] = op_store;

  vm->cpu.opers[CALL] = op_call;
  vm->cpu.opers[BACK] = op_back;

  vm->cpu.opers[LDFLG] = op_ldflg;
  vm->cpu.opers[STFLG] = op_stflg;
  vm->cpu.opers[LDCCR] = op_ldccr;
  vm->cpu.opers[STCCR] = op_stccr;
  vm->cpu.opers[LDICR] = op_ldicr;
  vm->cpu.opers[STICR] = op_sticr;
  vm->cpu.opers[LDTCR] = op_ldtcr;
  vm->cpu.opers[STTCR] = op_sttcr;
  vm->cpu.opers[LDACR] = op_ldacr;
  vm->cpu.opers[STACR] = op_stacr;

  vm->cpu.opers[LDCTX] = op_ldctx;
  vm->cpu.opers[STCTX] = op_stctx;

  vm->cpu.opers[DI] = op_di;
  vm->cpu.opers[EI] = op_ei;
  vm->cpu.opers[IBACK] = op_iback;

  vm->cpu.opers[PSHA] = op_psha;
  vm->cpu.opers[POPA] = op_popa;

  memset(vm->cpu.icache_addr, 0xff, sizeof(vm->cpu.icache_addr));
  vm->cpu.icache_oldest = 0;

  printf("  CPU - OK");
  return 1;
}

static uint32_t init_ram(vm_t *vm)
{
  uint32_t size = (uint32_t)sizeof(ram_t) * RAM_SIZE;
  vm->ram = (ram_t *)malloc(size);
  memset(vm->ram, 0, size);
  printf("  RAM (0x%x bytes) - %s\n", size, (vm->ram) ? "OK" : "ERR");
  return (vm->ram) ? 1 : 0;
}