#ifndef __ISA_H
#define __ISA_H

#include "vm.h"

#define MOV   0
#define EXT   1
#define EXTS  2
#define ADD   3
#define SUB   4
#define MUL   5
#define DIV   6
#define INC   7
#define DEC   8
#define OR    9
#define AND   10
#define XOR   11
#define SHL   12
#define SHR   13
#define NOT   14
#define CMP   15
#define CMPS  16
#define LOAD  17
#define STORE 18
#define PSH   19
#define POP   20
#define BR    21
#define BRE   22
#define BRNE  23
#define BRG   24
#define BRGE  25
#define BRL   26
#define BRLE  27
#define NOP   33
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