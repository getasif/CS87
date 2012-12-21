/*
 *  This is a good program to practice attaching gdb to a running program 
 *  (1) run the program
 *     (1b) type ps to get your program's pid (process identifier)
 *  (2) attach the debugger:   gdb attach <pid> infiniteloop
 *  (3) then type 'where' to see the execution stack
 *
 *  Once you fix the infinite loop, this is also a good program to
 *  try out valgrind...it contains all kinds of memory access errors.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void foo(char *str) {

  int i=0;

  while(i < 10) {
    if(str && (strlen(str) >= 10)){
      str[i] = str[0];
    }
  }
}


int main(int argc, char *argv[]) {

  char *s;

  s = (char *)malloc(sizeof(char)*8);
  strcpy(s, "abcdefg");

  foo(s);	  // calling foo with an allocated and initialized string  	
  printf("s = %s\n", s);

  // writing beyond the bounds of an array
  s[6] = 'a';
  s[7] = 'b';
  s[8] = 'c';
  printf("s = %s\n", s);


  free(s);  // deallocate space for s




  s = (char *)malloc(sizeof(char)*8);
  foo(s);	  // calling foo with an allocated but uninitialized string  	
  free(s);  // deallocate space for s
  free(s);  // freeing already free'ed memory 

  foo(s);	// calling foo with an unallocated and uninitialized string  	

  return 0;

}
