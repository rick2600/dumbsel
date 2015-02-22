#ifndef __CPU_H
#define __CPU_H

#define SWAP_UINT16(x) (((x) >> 8) | ((x) << 8))
#define SWAP_UINT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))

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
  int (*isa[64])();
  unsigned short int regs[16];
  unsigned short int pc; 
  unsigned int ir; 
  unsigned short int temp;
  unsigned char flags;
  unsigned char status;
  inst_t *inst;

} cpu_t;


void *cpu_uc(void *args);

#endif