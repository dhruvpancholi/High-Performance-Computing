/*
Author: Dhruv Pancholi
Program: Matrix Vector multiplication using MPI
*/

#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "mpi.h"

#ifndef SIZE
#define SIZE 4
#endif

int main(int argc, char *argv[])
{
	int p, my_rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	int n, local_n;
	double *A, *A_root, *x, *C, *C_root;

	//Allocating x on all the processors
	x=(double *)malloc(SIZE*sizeof(double));

	if (my_rank==0)
	{
		//Scanning the matrix A and allocating the memory only on the master processor
		A_root=(double *)malloc(SIZE*SIZE*sizeof(double));
		C_root=(double *)malloc(SIZE*sizeof(double));
		for (int i = 0; i < SIZE*SIZE; i++)
		{
			scanf("%lf",&A_root[i]);
		}
		
		for (int i = 0; i < SIZE; i++)
		{
			scanf("%lf",&x[i]);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Bcast(x, SIZE, MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);

	local_n=SIZE/p;
	A=(double *)malloc(SIZE*local_n*sizeof(double));
	C=(double *)malloc(SIZE*sizeof(double));

	//Scattering the matrix to different processors
	MPI_Scatter(A_root, SIZE*local_n, MPI_DOUBLE, A, SIZE*local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	for (int i = 0; i < SIZE/p; i++)
	{
		C[i]=0;
		for (int j = 0; j < local_n; j++)
		{
			C[i]+=A[i*SIZE+j]*x[j];
		}
	}
	
	//Finally gathering all the elements on the master thread
	MPI_Gather(C,SIZE,MPI_DOUBLE,C_root,SIZE,MPI_DOUBLE,0,MPI_COMM_WORLD);
	if (my_rank==0)
	{
		for (int i = 0; i < SIZE; i++)
		{
			printf("%lf\n", C_root[i]);
		}
	}
	
	MPI_Finalize();
	return 0;
}