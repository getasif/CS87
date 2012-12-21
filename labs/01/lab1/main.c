/*
 * Add a top level comment:
 * (1) high-level summary of program 
 * (2) your names and date
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "counter.h"

int main(int argc, char *argv[]) {

  int i;

  // access a global variable 
  my_counter = 8;

  // call a global function
  for (i=0; i < 5; i++) {
     printf("next counter value: %d\n", inc_and_return_count());
  }

  exit(0);

}
