#include <stdio.h>

void tryit(
 int * x1,
 const int * x2)
{
  *x1 = 4; /* also tried with *x2 = 4, which produces warning or error */
}

main()
{
  int x;

  x = 3;
  tryit(&x, (const int *)&x); /* can omit (const int *). Behaves the same */
  printf("x = %d\n", x); /* prints "x = 4" */
}
