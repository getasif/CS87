#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int foo(char *str) {

  char a[5];	// unfortunately, valgrind and purify can't find 
                // mem access errors with variables statically 
                // allocated on the stack
  char b[5];

  char *c;	// but it can find such errors with dynamically
                // allocated memory (heap memory) 
  int i;

  strcpy(a, "aaaa"); 	// bad code that doesn't check return values 
  strcpy(b, "bbbb");
  c = (char *)malloc(sizeof(char)*5);
  strcpy(c, "cccc");


  for(i=0; i <=5; i++) {
    a[i] = str[i];      // here we are going beyond the bounds of
                        // of "a", but valgrind can't detect it
                        // because a the array is on the stack
  }

  for(i=0; i <=5; i++) {
    c[i] = str[i];      // here we are going beyond the bounds of
                        // of the array, and valgrind can detect 
                        // it because the array is in the heap
  }

  printf("a = %s, b = %s c = %s\n", a, b, c);

  // hey, we are forgeting to free malloced space for c before
  // exiting this function: this is a memory leak
  return 0;	
}


int main(int argc, char *argv[]) {

  char *s;

  s = (char *)malloc(sizeof(char)*8);
  strcpy(s, "ssssss");

  foo(s);  // calling foo with an allocated and initialized string  	
  printf("s = %s\n", s);
  free(s);  // deallocate space for s

  s = (char *)malloc(sizeof(char)*8);
  foo(s);   // calling foo with an allocated but uninitialized string  	
  free(s);  // deallocate space for s
  foo(s);   // calling foo with an unallocated and uninitialized string  	

  s = (char *)malloc(sizeof(char)*8);
  strcpy(s, "xxxx");
  printf("s = %s\n", s);
  s = (char *)malloc(sizeof(char)*8); // a memory leak...didn't free xxxx str
  printf("s = %s\n", s);  // uninitialized read
  free(s);  
  free(s);  // freeing already free'ed memory 

  return 1;
}
