#ifndef __ISA_H
#define __ISA_H

#include "vm.h"

#define NOP   0
#define MOV   1
#define EXT   2
#define EXTS  3
#define ADD   4
#define SUB   5
#define MUL   6
#define DIV   7
#define INC   8
#define DEC   9
#define OR    10
#define AND   11
#define XOR   12
#define SHL   13
#define SHR   14
#define NOT   15
#define CMP   16
#define CMPS  17
#define LOAD  18
#define STORE 19
#define PSH   20
#define POP   21
#define BR    22
#define BRE   23
#define BRNE  24
#define BRG   25
#define BRGE  26
#define BRL   27
#define BRLE  28
#define HLT   35


#define ZF(x) ((x) & 1)
#define LT(x) (((x) >> 1) & 1)
#define GT(x) (((x) >> 2) & 1)

#define SET_ZF(x) ((x) | 1)
#define SET_LT(x) ((x) | 2)
#define SET_GT(x) ((x) | 4)

#define CLR_ZF(x) ((x) & ~1)
#define CLR_LT(x) ((x) & ~2)
#define CLR_GT(x) ((x) & ~4)



int isa_mov(vm_t *vm);
int isa_ext(vm_t *vm);
int isa_exts(vm_t *vm);
int isa_add(vm_t *vm);
int isa_sub(vm_t *vm);
int isa_mul(vm_t *vm);
int isa_div(vm_t *vm);
int isa_inc(vm_t *vm);
int isa_dec(vm_t *vm);
int isa_not(vm_t *vm);

int isa_or(vm_t *vm);
int isa_and(vm_t *vm);
int isa_xor(vm_t *vm);
int isa_shl(vm_t *vm);
int isa_shr(vm_t *vm);

int isa_cmp(vm_t *vm);
int isa_cmps(vm_t *vm);

int isa_load(vm_t *vm);
int isa_store(vm_t *vm);

int isa_psh(vm_t *vm);
int isa_pop(vm_t *vm);
int isa_hlt(vm_t *vm);
int isa_nop(vm_t *vm);
int isa_br(vm_t *vm);
int isa_bre(vm_t *vm);
int isa_brne(vm_t *vm);
int isa_brg(vm_t *vm);
int isa_brge(vm_t *vm);
int isa_brl(vm_t *vm);
int isa_brle(vm_t *vm);





#endif