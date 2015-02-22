#include <stdio.h>
#include <stdlib.h>
#include "vm.h"


int main(int argc, char **argv)
{

  vm_t *vm;

  if (argc != 2)
  {
    printf("usage: %s <code.bin>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  vm = create_vm();

  turn_on(vm, argv[1]);
  turn_off(vm);
  
  exit(EXIT_SUCCESS);
}
