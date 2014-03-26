/*
Author: Dhruv Pancholi
Program: Matrix Vector multiplication using MPI
*/

#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "mpi.h"

int main(int argc, char *argv[])
{
	int p, my_rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	int n, local_n;
	double *A;
	if (my_rank==0)
	{
		printf("Enter the size of the matrix:");
		scanf("%d",&n);

		//Scanning the matrix A
		A=(double *)malloc(n*n*sizeof(double));
		for (int i = 0; i < n*n; i++)
		{
			scanf("%lf",&A[i]);
		}
	}
	else{
		A=(double *)malloc(n*n*sizeof(double)/p);
	}
	MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
	local_n=n/p;
	//Scattering the matrix to different processors
	//MPI_Scatter(A,n*n,MPI_DOUBLE,A,n*n/p,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}