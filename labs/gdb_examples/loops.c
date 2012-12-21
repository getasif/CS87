/*
 *  This is a good program to practice adding conditional breakpoints
 *  try setting a conditional breakpoint on the loop counter variable
 *  in foo (like when i > 100, for example):
 *
 *  (gdb) break 28
 *  (gdb) info break
 *    Num Type           Disp Enb Address    What
 *    1   breakpoint     keep y   0x080483a3 in foo at loops.c:28
 *  (gdb) condition 1 (i > 100)
 *  (gdb) run   (or continue if already running) 
 *  then when hit conditional break point print out the value of i to see
 *  that it is > 100
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * bad function 
 */
int foo(int val) {

  int i=0;

  for(i=0; i < 1000; i+= 10) {
    if(i > val) 
      return i;
  }
  return 0;
}


int main(int argc, char *argv[]) {

  printf("the first mult of 10 > %d is %d\n", 200, foo(200));	  
  printf("the first mult of 10 > %d is %d\n", 1000, foo(1000));	  
  return 0;
}
