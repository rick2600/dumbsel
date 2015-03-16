#ifndef __DEBUG_H
#define __DEBUG_H

#include "vm.h"

void run_debugger(vm_t *vm, uint32_t r, uint32_t m, uint32_t i, uint32_t stop);
void run_debugger2(vm_t *vm, uint32_t stop);



#endif