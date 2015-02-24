#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "vm.h"

vm_t *vm;

void handler(int sig);

int main(int argc, char **argv)
{

  vm = NULL;

  if (argc != 2)
  {
    printf("usage: %s <code.bin>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  signal(SIGINT, handler);
  vm = create_vm();

  turn_on(vm, argv[1]);
  turn_off(vm);
  
  exit(EXIT_SUCCESS);
}


void handler(int sig)
{  
  if (vm && vm->cpu)
  {
    printf("\npress enter to exit\n");
    vm->cpu->halt = 1;
  }
}