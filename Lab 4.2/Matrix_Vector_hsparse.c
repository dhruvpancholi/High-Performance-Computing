/*
 *   Computational Project 4.2
 *   Matrix-Matrix multiplication
 *
 *   This program multiplies two matrices input from
 *   separate files. The result vector is printed to standard
 *   output.
 *
 *   Data distribution of matrix: rowwise block striped
 *   Data distribution of vector: replicated 
 *
 *   Programmed by Dhruv Pancholi
 *
 *   Last modification: 26 March 2014
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define TAG 17

int main(int argc, char *argv[]) {
  double **A, *B, *C, *tmp;
  double startTime, endTime;
  int numElements, offset, stripSize, my_rank, p, N, i, j, k;
  int *recv_size;
  int *index;
  
  MPI_Init(&argc, &argv);
  
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  recv_size=malloc(p*sizeof(int));
  N=10;
  if (argv[1]!=NULL)
  {
    N = atoi(argv[1]);
  }

  /*
  Creating a two dimensional array to work upon
  Every element in array A is pointing to an array which is contigous
  */
  if (my_rank == 0) {
    tmp = (double *) malloc (sizeof(double ) * N * N);
    A = (double **) malloc (sizeof(double *) * N);
    for (i = 0; i < N; i++)
      A[i] = &tmp[i * N];
  }
  else {
    /*
    On other processors creating matrix of size N/p*N/p
    */
    tmp = (double *) malloc (sizeof(double ) * N * N / p);
    A = (double **) malloc (sizeof(double *) * N / p);
    for (i = 0; i < N / p; i++)
      A[i] = &tmp[i * N];
  }
  
  
  B = (double *) malloc (sizeof(double) * N);  
  
  if (my_rank == 0) {
    C = (double *) malloc (sizeof(double) * N);
  }
  else {
    C = (double *) malloc (sizeof(double) * N / p);
  }

  if (my_rank == 0) {
    // initialize A and B
    for (i=0; i<N; i++) {
      for (j=0; j<N; j++) {
        A[i][j] = 1.0;
      }
      B[i] = 1.0;
    }
  }
  
  // start timer
  if (my_rank == 0) {
    startTime = MPI_Wtime();
  }
  
  stripSize = N/p;

  // send each node its piece of A -- note could be done via MPI_Scatter
  if (my_rank == 0) {
    offset = stripSize;
    numElements = stripSize * N;
    for (i=1; i<p; i++) {
      recv_size[i]=0;
      for (int j = 0; j < numElements; j++)
      {
        if (*(A[offset]+j)!=0)
        {
          recv_size[i]++;
        }
      }
      index=malloc(recv_size[i]*sizeof(int));
      int count=0;
      for (int j = 0; j < numElements; j++)
      {
        if (*(A[offset]+j)!=0)
        {
          index[count]=j;
          count++;
        }
      }
      MPI_Send(A[offset], numElements, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD);
      //MPI_Send(index, recv_size[i], MPI_INT, i, TAG+1, MPI_COMM_WORLD);
      offset += stripSize;
      free(index);
    }
    // For the process 0
    for (int j = 0; j < numElements; j++)
      {
        recv_size[0]=0;
        if (*(A[0]+j)!=0)
        {
          recv_size[0]++;
        }
      }
      index=malloc(recv_size[0]*sizeof(int));
      int count=0;
      for (int j = 0; j < numElements; j++)
      {
        if (*(A[0]+j)!=0)
        {
          index[count]=j;
          count++;
        }
      }
  }

  MPI_Bcast(recv_size, p, MPI_INT, 0, MPI_COMM_WORLD);

  if (my_rank!=0) {  // receive my part of A
    MPI_Recv(A[0], stripSize * N, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //index=malloc(recv_size[my_rank]*sizeof(int));
    //MPI_Recv(index, recv_size[my_rank], MPI_INT, 0, TAG+1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  
  // everyone gets B
  MPI_Bcast(B, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  // Let each process initialize C to zero 
  for (i=0; i<stripSize; i++) {
    C[i] = 0.0;
  }

  // do the work
  for (i=0; i<stripSize; i++) {
    for (j=0; j<N; j++) {
      C[i] += A[i][j] * B[j];
    }
  }

  // master receives from workers  -- note could be done via MPI_Gather
  if (my_rank == 0) {
    offset = stripSize; 
    numElements = stripSize;
    for (i=1; i<p; i++) {
      MPI_Recv(C+offset, numElements, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      offset += stripSize;
    }
  }
  else { // send my contribution to C
    MPI_Send(C, stripSize, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
  }

  // stop timer
  if (my_rank == 0) {
    endTime = MPI_Wtime();
    printf("Time is %f\n", endTime-startTime);
  }
  
  // print out matrix here, if I'm the master
  if (my_rank == 0 && N <= 12) {
    for (i=0; i<N; i++) {
        printf("%f ", C[i]);
    }
  }
  
  MPI_Finalize();
  return 0;
}

