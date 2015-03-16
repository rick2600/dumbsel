#ifndef __op_H
#define __op_H

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

#define LDCTX 30
#define STCTX 31
#define CALL  32
#define BACK  33
#define HLT   35
#define LDFLG 36
#define STFLG 37
#define LDCCR 38
#define STCCR 39
#define LDICR 40
#define STICR 41
#define LDTCR 42
#define STTCR 43
#define DI    44
#define EI    45
#define IBACK 46
#define PSHA  47
#define POPA  48
#define LDACR 49
#define STACR 50

// FLAGS

#define ZF (vm->cpu.flags & 1)
#define LT ((vm->cpu.flags >> 1) & 1)
#define GT ((vm->cpu.flags >> 2) & 1)
#define CF ((vm->cpu.flags >> 3) & 1)
#define SF ((vm->cpu.flags >> 4) & 1)
#define OF ((vm->cpu.flags >> 5) & 1)

#define SET_ZF (vm->cpu.flags |= 1)
#define SET_LT (vm->cpu.flags |= 2)
#define SET_GT (vm->cpu.flags |= 4)
#define SET_CF (vm->cpu.flags |= 8)
#define SET_SF (vm->cpu.flags |= 16)
#define SET_OF (vm->cpu.flags |= 32)

#define CLR_ZF (vm->cpu.flags &= ~1)
#define CLR_LT (vm->cpu.flags &= ~2)
#define CLR_GT (vm->cpu.flags &= ~4)
#define CLR_CF (vm->cpu.flags &= ~8)
#define CLR_SF (vm->cpu.flags &= ~16)
#define CLR_OF (vm->cpu.flags &= ~32)



// CCR - CPU CONTROL REGISTER

#define INTERRUPTION_ENABLED(x)    ((x) & 1)
#define IN_HALT_STATE(x)           (((x) >> 1) & 1)
#define IN_SUPERVISOR_MODE(x)      (((x) >> 2) & 1)
#define VIRTUAL_MODE_ENABLED(x)    (((x) >> 3) & 1)

#define ENABLE_INTERRUPTION    (vm->cpu.ccr |= 1)
#define ENTER_HALT_STATE       (vm->cpu.ccr |= 2)
#define ENTER_SUPERVISOR_MODE  (vm->cpu.ccr |= 4)
#define ENTER_VIRTUAL_MODE     (vm->cpu.ccr |= 8)

#define DISABLE_INTERRUPTION   (vm->cpu.ccr &= ~1)
#define LEAVE_HALT_STATE       (vm->cpu.ccr &= ~2)
#define ENTER_USER_MODE        (vm->cpu.ccr &= ~4)
#define LEAVE_VIRTUAL_MODE     (vm->cpu.ccr &= ~8)



/*
#define ZF(x) ((x) & 1)
#define LT(x) (((x) >> 1) & 1)
#define GT(x) (((x) >> 2) & 1)
#define CF(x) (((x) >> 3) & 1)
#define SF(x) (((x) >> 4) & 1)
#define OF(x) (((x) >> 5) & 1)

#define SET_ZF(x) ((x) | 1)
#define SET_LT(x) ((x) | 2)
#define SET_GT(x) ((x) | 4)
#define SET_CF(x) ((x) | 8)
#define SET_SF(x) ((x) | 16)
#define SET_OF(x) ((x) | 32)

#define CLR_ZF(x) ((x) & ~1)
#define CLR_LT(x) ((x) & ~2)
#define CLR_GT(x) ((x) & ~4)
#define CLR_CF(x) ((x) & ~8)
#define CLR_SF(x) ((x) & ~16)
#define CLR_OF(x) ((x) & ~32)


#define CCR_INT_ENABLED(x)     ((x) & 1)
#define CCR_HALT(x)            (((x) >> 1) & 1)
#define CCR_SUPERVISOR(x)      (((x) >> 2) & 1)

#define CCR_INT_ENABLE(x)      ((x) | 1)
#define CCR_SET_HALT(x)        ((x) | 2)
#define CCR_SET_SUPERVISOR(x)  ((x) | 4)


#define CCR_INT_DISABLE(x)     ((x) & ~1)
#define CCR_CLR_HALT(x)        ((x) & ~2)
#define CCR_CLR_SUPERVISOR(x)  ((x) & ~4)



*/



int __op_psh(vm_t *vm, unsigned short v);
int cpu_w_mem(vm_t *vm, uint16_t mar, uint32_t mdr, uint32_t m);
int cpu_r_mem(vm_t *vm, uint16_t mar, uint32_t *data);


void op_mov(vm_t *vm);
void op_ext(vm_t *vm);
void op_exts(vm_t *vm);
void op_add(vm_t *vm);
void op_sub(vm_t *vm);
void op_mul(vm_t *vm);
void op_div(vm_t *vm);
void op_inc(vm_t *vm);
void op_dec(vm_t *vm);
void op_not(vm_t *vm);

void op_or(vm_t *vm);
void op_and(vm_t *vm);
void op_xor(vm_t *vm);
void op_shl(vm_t *vm);
void op_shr(vm_t *vm);

void op_cmp(vm_t *vm);
void op_cmps(vm_t *vm);

void op_load(vm_t *vm);
void op_store(vm_t *vm);

void op_psh(vm_t *vm);
void op_pop(vm_t *vm);
void op_hlt(vm_t *vm);
void op_nop(vm_t *vm);
void op_br(vm_t *vm);
void op_bre(vm_t *vm);
void op_brne(vm_t *vm);
void op_brg(vm_t *vm);
void op_brge(vm_t *vm);
void op_brl(vm_t *vm);
void op_brle(vm_t *vm);
void op_call(vm_t *vm);
void op_back(vm_t *vm);
void op_ldflg(vm_t *vm);
void op_stflg(vm_t *vm);

void op_ldccr(vm_t *vm);
void op_stccr(vm_t *vm);

void op_ldicr(vm_t *vm);
void op_sticr(vm_t *vm);

void op_ldtcr(vm_t *vm);
void op_sttcr(vm_t *vm);

void op_ldacr(vm_t *vm);
void op_stacr(vm_t *vm);

void op_ldctx(vm_t *vm);
void op_stctx(vm_t *vm);

void op_di(vm_t *vm);
void op_ei(vm_t *vm);

void op_iback(vm_t *vm);

void op_psha(vm_t *vm);
void op_popa(vm_t *vm);



#endif
