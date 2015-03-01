#ifndef __CPU_H
#define __CPU_H

#define SWAP_UINT16(x) (((x) >> 8) | ((x) << 8))
#define SWAP_UINT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))

#define ICACHE_SIZE 16

typedef enum
{
  INT_TIME_EXPIRATION
} cpu_int_t;


typedef struct
{
  unsigned int op; 
  unsigned char has_imm;
  unsigned char byte_mode;
  unsigned char ra; 
  unsigned char rb; 
  unsigned char rc; 
  unsigned short imm;

} inst_t;



typedef struct 
{
  // instructions
  int (*isa[64])();

  // context
  unsigned short int regs[16];
  unsigned short int pc; 
  unsigned short int temp;
  unsigned short int flags;
  unsigned char status;
  unsigned char halt;

  // cache
  unsigned int icache_data[ICACHE_SIZE];
  unsigned int icache_addr[ICACHE_SIZE];
  unsigned char icache_oldest;

  // intruction decoding
  unsigned int ir; 
  inst_t *inst;

  // task table
  unsigned short int ttr;

  // interruption handler table
  unsigned short int ihtr;

} cpu_t;


void *cpu_uc(void *args);

#endif