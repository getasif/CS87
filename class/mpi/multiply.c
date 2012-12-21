/*
 * first attempt at simple MPI program:
 *
 *       A = B * C     (arrays)
 *
 */

#include "mpi.h"
#include "math.h"
#include <stdio.h>

#define N 100

int main( int argc, char *argv[] )
{
  int numtasks, rank, rc, reslen;
  char name[MPI_MAX_PROCESSOR_NAME];

  int i, n;
  int A[N],B[N],C[N],recvmsg[N];
  float dx;
  int idx, start, stop;
  MPI_Status stat;

  for (i=0; i<N; i++) B[i] = i;
  for (i=0; i<N; i++) C[i] = 2;

/////////////////// --- parallel code --- ////////////////////
/////////////////// --- parallel code --- ////////////////////

  MPI_Init( &argc, &argv );

  MPI_Get_processor_name(name, &reslen);
  MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

// divide up work between procs; calc array start and stop indecies
  dx = 1.0*N/numtasks;
  idx = ceil(dx);    // round up
  start = rank*idx;
  stop = start + (idx - 1);
  if (stop >= N) { stop = N-1; }
  printf ("(%d) %10s..numtasks= %d   (%4d,%4d)\n", rank, name, numtasks, start,stop);

  // each task just updates their part of A
  for (i=start; i<=stop; i++) {
    A[i] = B[i] + C[i] + rank;
    if (i == stop-1) {
      printf("A[%d] = %d -------- %s\n", i, A[i], name);
    }
  }

  // how to get results back into A ???

  if (rank == 0) {
    // receive and process all chunks
    for (n=1; n<numtasks; n++) {
      MPI_Recv(recvmsg,sizeof(int),MPI_INT,n,321,MPI_COMM_WORLD,&stat); 
      printf ("task %d sent this: \n", n);
      printf("recvmsg[%d] = %d..", 0, recvmsg[0]);
      printf ("\n---------------------\n");
    }
  } else {
    // send my data to task 0
    MPI_Send(A[start], sizeof(int), MPI_INT, 0, 321, MPI_COMM_WORLD);
  }

  MPI_Finalize();

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

  // see if we got correct results...
  for (i=0; i<=N; i=i+idx) {
    printf("i=%4d ... A[i] = %4d\n", i, A[i]);
  }

  return 0;
}
