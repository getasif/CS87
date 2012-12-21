/* 
 * in this example try setting break points in function foo, blah, f, and g
 * the print out the stack contents when you reach each break point
 *
 * check what you know about local variables and pass by value by
 * examining variables in different stack frames
 *
 * step through a function call and a function return to see what happens
 *
 */
#include <stdio.h>
#include <stdlib.h>

int g(int x){
  x += 20;
  printf("        in g: x = %d\n", x);
  return( x );
}

void f(int y){

  printf("      in f before g: y = %d\n", y);
  y = g(y+1);
  printf("      in f after g: y = %d\n", y);
}



void blah(int *y) {

  *y = *y + 30;
  printf("    in blah before f: y = %d\n", *y);
  f(*y); 
  printf("    in blah after f: y = %d\n", *y);
}

int foo(int x) {

  printf("  in foo before blah: x = %d\n", x);
  blah(&x);
  printf("  in foo after blah: x = %d\n", x);
  return x;

}


int main(int argc, char *argv[]) {

  int x, i;
  for(i=0; i < 3; i++) {
    x = 10+i;
    printf("in main before foo: x = %d\n", x);
    x = foo(x); 
    printf("in main after foo: x = %d\n", x);
  }
  return 0;
}
