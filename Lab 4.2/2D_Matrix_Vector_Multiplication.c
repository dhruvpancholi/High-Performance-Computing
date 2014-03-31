/*
* Source: Program 6.8 from Introduction to Parallel Computing by Grama, Gupta, Karypis and Kumar- 2003
* Modified by: Dhruv Pancholi
* Matrix Vector Multiplication
*/
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>

void MatrixVectorMultiply_2D(int n, double *a, double *b, double *FinalVector, MPI_Comm comm);

main(int argc, char** argv) 
{
int n =16;
int p,myrank,b;
double * A;
double * B;
double * x;


MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
MPI_Comm_size(MPI_COMM_WORLD, &p);

A = (double *)malloc(n*n*sizeof(double));
B = (double *)malloc(n*sizeof(double));
x = (double *)malloc(n*sizeof(double));
int nproc;
nproc = sqrt(p);

if(myrank==0)
{
int i,j;
/*Size of Matrix*/
FILE* fp=fopen("16.txt","r");
fscanf(fp,"%d",&n);



  for(i = 0; i< n; i++) 
  {
      for(j = 0; j < n; j++) 
       	 {
          fscanf(fp,"%lf",&A[i*n+j]);
       	 }	
   	 }

   	for (i = 0; i<n;i++)
   	 {
   	 fscanf(fp,"%lf", &B[i]);
   	 }
fclose(fp);
}


 	MPI_Bcast(B, n, MPI_DOUBLE,0, MPI_COMM_WORLD);
 	MPI_Barrier(MPI_COMM_WORLD);
 	printf("B bcased\n");

   MPI_Bcast(A,n*n, MPI_DOUBLE,0, MPI_COMM_WORLD);
   MPI_Barrier(MPI_COMM_WORLD);
   printf("A bcased\n");

MPI_Bcast(x,n, MPI_DOUBLE,0, MPI_COMM_WORLD);
MPI_Barrier(MPI_COMM_WORLD);
printf("x bcased\n");


MatrixVectorMultiply_2D(n,A,B,x,MPI_COMM_WORLD);


return 0;
}


void MatrixVectorMultiply_2D(int n, double *A, double *B, double *FinalVector, MPI_Comm comm)
{

int ROW=0, COL=1; /*improve readability*/

int i,j,nlocal;


double *px; /*Will store partial dot products*/

int npes, dims[2], periods[2], keep_dims[2];

int myrank, my2drank,mycoords[2];

int other_rank,coords[2];

MPI_Status status;
MPI_Comm comm_2d, comm_row, comm_col;

/*Get information about the communicator*/

MPI_Comm_size(comm,&npes);
MPI_Comm_rank(comm,&myrank);

/*Compute the size of the square grid*/

dims[ROW]=dims[COL]=sqrt(npes);

nlocal= n/dims[ROW];

/*Allocate memory for the array that will hold the partial dot-products*/

px= malloc(nlocal*sizeof(double));

/*Set up the Cartesian Topology and get the rank and coordinates
of the process in this topology */

periods[ROW]=periods[COL]=1;/*Set the periods for wrap-around connections*/

MPI_Cart_create(MPI_COMM_WORLD, 2,dims,periods,1,&comm_2d);

MPI_Comm_rank(comm_2d, &my2drank);	  /*Get my rank in the new topology*/

MPI_Cart_coords(comm_2d,my2drank,2,mycoords); /*Get my coordinates*/

/*Create the row-based sub-topology*/
keep_dims[ROW]=0;
keep_dims[COL]=1;

MPI_Cart_sub(comm_2d,keep_dims,&comm_row);

/*Create the column-based sub-topology*/
keep_dims[ROW]=1;
keep_dims[COL]=0;
MPI_Cart_sub(comm_2d,keep_dims, &comm_col);

/*Redistribute the b vector*/
/*Step 1. The processors along the 0th column send their data to the
diagonal processors*/

double a[nlocal*nlocal];
double b[nlocal];
if(mycoords[COL]==0)
{
for (i = 0; i < nlocal; i++)
{	
for (j=0; j < nlocal; j++)
{
b[j] = B[i*mycoords[COL]+j];	
}

}
}

for (i =0; i<nlocal;i++)
{	
for (j =0;j<nlocal;j++)
{	
a[i*nlocal+j] = A[n*nlocal*mycoords[COL]+n*i+nlocal*mycoords[COL]+j];
}	
}
printf("a and b assigned\n");
MPI_Barrier( comm_2d );
MPI_Barrier( comm_col );
MPI_Barrier( comm_row );
if (mycoords[COL]==0 && mycoords[ROW] !=0) /* At 1st column*/
{
coords[ROW]=mycoords[ROW];
coords[COL]=mycoords[ROW];
//coords[COL]=mycoords[COL];
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
printf("b sent to diagonal processors\n");
/*Step 2. The diagonal processors perform a column-wise broadcast*/

coords[0]=mycoords[COL];
   MPI_Cart_rank(comm_col, coords, &other_rank);

MPI_Barrier( comm_2d );
MPI_Barrier( comm_col );
MPI_Barrier( comm_row );

MPI_Bcast(b, nlocal, MPI_DOUBLE, other_rank, comm_col);

MPI_Barrier( comm_2d );
MPI_Barrier( comm_col );
MPI_Barrier( comm_row );
printf("diagonal processors bcasted into column and computation begins\n");
/*Get into the main computational loop*/
int q,l;
for (q=0; q<nlocal;q++)
{
px[q]=0.0;
for (l=0;l<nlocal;l++)
px[l] = px[l]+a[q*nlocal+l]*b[l];
}

/*Perform the sum reduction along the rows to add up the partial
dot products */
coords[0]=0;

MPI_Cart_rank(comm_row, coords, &other_rank);
printf("reduction begins\n");
MPI_Barrier( comm_2d );
MPI_Barrier( comm_col );
MPI_Barrier( comm_row );
MPI_Reduce(px,FinalVector,nlocal,MPI_DOUBLE, MPI_SUM, other_rank, comm_row);
printf("reduction ends\n");
/*	MPI_Comm_free(&comm_2d);
MPI_Comm_free(&comm_row);
   MPI_Comm_free(&comm_col);
free(px);*/
}