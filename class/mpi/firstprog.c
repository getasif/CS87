
#include "mpi.h"
#include <stdio.h>

int main( int argc, char *argv[] )
{
  int numtasks, rank, rc;
  int my_name_length;
  char my_name[256];

  MPI_Init( &argc, &argv );

  MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Get_processor_name(my_name, &my_name_length);
  printf ("(%10s)..number of tasks= %d My rank= %d\n", my_name,numtasks,rank);
  printf( "Hello, world!\n" );

  MPI_Finalize();
  return 0;
}

