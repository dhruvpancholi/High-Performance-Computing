/*
* Author: Dhruv Pancholi
* Matrix Vector Multiplication
*/
#include "stdio.h"
#include "mpi.h"
#include "math.h"
#include "stdlib.h"


void MatrixVectorMultiply_2D(int n, double *a, double *b, double *x, MPI_Comm comm)
{
	/*Index of the variables used in the creation of Cartesian Topology*/
	int ROW=0, COL=1; /*improve readability*/

	int i,j,nlocal;
	double *px; /*Will store partial dot products*/
	int npes, dims[2], periods[2], keep_dims[2];

	/*Tracking the rank, rank in Cartesian Topology and coordinates in the cartesian topology respectively*/
	int myrank, my2drank,mycoords[2];
	int other_rank,coords[2];
	MPI_Status status;

	/*2d Topology communicator, row of the communicator and column of the communicator*/
	MPI_Comm comm_2d, comm_row, comm_col;

	/*Get information about the communicator*/
	MPI_Comm_size(comm,&npes);
	MPI_Comm_rank(comm,&myrank);

	/*Compute the size of the square grid*/
	dims[ROW]=dims[COL]=sqrt(npes);
	nlocal=n/dims[ROW];

	/*Allocate memory for the array that will hold the partial dot-products*/
	px= malloc(nlocal*sizeof(double));

	/*Set up the Cartesian Topology and get the rank and coordinates
	of the process in this topology */
	periods[ROW]=periods[COL]=1;/*Set the periods for wrap-around connections*/

	MPI_Cart_create(MPI_COMM_WORLD, 2,dims,periods,1,&comm_2d);
	MPI_Comm_rank(comm_2d, &my2drank);/*Get my rank in the new topology*/
	MPI_Cart_coords(comm_2d,my2drank,2, mycoords); /*Get my coordinates*/

	/*Create the row-based sub-topology*/
	keep_dims[ROW]=0;
	keep_dims[COL]=1;
	MPI_Cart_sub(comm_2d,keep_dims, &comm_row);

	/*Create the column-based sub-topology*/
	keep_dims[ROW]=1;
	keep_dims[COL]=0;
	MPI_Cart_sub(comm_2d,keep_dims, &comm_row);

	/*Redistribute the b vector*/
	/*Step 1. The processors along the 0th column send their data to the
	diagonal processors*/
	if (mycoords[COL]==0 && mycoords[ROW] !=0) /* At 1st column*/
	{
		coords[ROW]=mycoords[ROW];
		coords[COL]=mycoords[COL];
		MPI_Cart_rank(comm_2d, coords, &other_rank);
		MPI_Send(b, nlocal, MPI_DOUBLE, other_rank, 1, comm_2d);
	}
	if (mycoords[ROW] == mycoords[COL] && mycoords[ROW]!=0)
	{
        coords[ROW]=mycoords[ROW];
		coords[COL]=0;
		MPI_Cart_rank(comm_2d, coords, &other_rank);
		MPI_Recv(b, nlocal, MPI_DOUBLE, other_rank, 1, comm_2d, &status);
	}
	/*Step 2. The diagonal processors perform a column-wise broadcast*/
	coords[0]=mycoords[COL];
    MPI_Cart_rank(comm_2d, coords, &other_rank);
	MPI_Bcast(b, nlocal, MPI_DOUBLE, other_rank, comm_col);

	/*Get into the main computational loop*/
	for (i=0; i<nlocal;i++)
	{
		px[i]=0.0;
		for (j=0;j<nlocal;j++)
			px[i] +=a[i*nlocal+j]*b[j];
	}

	/*Perform the sum reduction along the rows to add up the partial
	dot products */
	coords[0]=0;
	MPI_Cart_rank(comm_row, coords, &other_rank);
	MPI_Reduce(px,x,nlocal,MPI_DOUBLE, MPI_SUM, other_rank, comm_row);

	MPI_Comm_free(&comm_2d);
	MPI_Comm_free(&comm_row);
    MPI_Comm_free(&comm_col);
	free(px);
}
int main(int argc, char *argv[])
{
	int p;
	int my_rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	double *a,*b,*x;
	int n=10;
	a=malloc(n*sizeof(double));b=malloc(n*sizeof(double));x=malloc(n*sizeof(double));
	for (int i = 0; i < n; i++)
	{
		a[i]=i;
		b[i]=i*i;
	}
	MPI_Comm comm;
	MPI_Comm_dup(MPI_COMM_WORLD, &comm);
	MatrixVectorMultiply_2D(n,a,b,x,comm);
	MPI_Finalize();
	return 0;
}
