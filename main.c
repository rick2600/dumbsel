#include <stdio.h>
#include <stdlib.h>
#include "vm.h"


int main(int argc, char **argv)
{

  vm_t *vm = create_vm();

  turn_on(vm);
  sleep(2);
  turn_off(vm);
  
  exit(EXIT_SUCCESS);
}
