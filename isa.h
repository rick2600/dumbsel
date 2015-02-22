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

#define LOAD  17

#define POP   20
#define HLT   35


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

int isa_load(vm_t *vm);
int isa_pop(vm_t *vm);
int isa_hlt(vm_t *vm);




#endif