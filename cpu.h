#ifndef __CPU_H
#define __CPU_H

#define SWAP_UINT16(x) (((x) >> 8) | ((x) << 8))
#define SWAP_UINT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))

#define ICACHE_SIZE 16
#define QUANTUM      8

#include <stdint.h>

typedef enum
{
  QUANTUM_EXPIRED

} cpu_int_t;


typedef struct
{
  uint32_t op; 
  uint8_t i;
  uint8_t bmode;
  uint8_t ra; 
  uint8_t rb; 
  uint8_t rc; 
  unsigned short imm;

} inst_t;



typedef struct 
{
  // instructions
  void (*opers[64])();

  // context
  uint16_t regs[16];
  uint16_t pc; 
  uint16_t flags;

  // last task context when entered in supervisor mode
  uint16_t _regs[16];
  uint16_t _pc; 
  uint16_t _flags;


  // cache
  uint32_t icache_data[ICACHE_SIZE];
  uint32_t icache_addr[ICACHE_SIZE];
  uint32_t icache_oldest;

  // intruction decoding
  uint32_t ir; 
  inst_t inst;

  // task control register
  uint16_t tcr;

  // interruption control register
  uint16_t icr;

  // cpu control register
  uint16_t ccr;

  // address control register
  uint16_t acr;

  uint8_t time_slice;

} cpu_t;


void *cpu_uc(void *args);

#endif