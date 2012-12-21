/******************************************************************************
* FILE: darts.c
* DESCRIPTION: main prog to test dboards.c
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double dboard(int darts);

int main(int argc, char **argv) {
  double homepi;
  char buf[BUFSIZ];
  int DARTS;
  double start, finish, elapsed;

  printf ("\nnumber of DARTS: ");
  if (fgets(buf, sizeof(buf), stdin) != NULL) { DARTS = atoi(buf); }  

  start = (double) clock() / CLOCKS_PER_SEC;
  homepi = dboard(DARTS);
  finish = (double) clock() / CLOCKS_PER_SEC;
  elapsed = finish - start;

  printf("\nFor %d darts, pi = %0.20f", DARTS, homepi);
  printf("elapsed time = %.3f sec\n\n", elapsed);

}
