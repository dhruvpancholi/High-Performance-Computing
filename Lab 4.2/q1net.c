/*
* Author: Dhruv Pancholi
* Matrix Vector Multiplication
*/
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>

main(int argc,char *argv[]) 
{ 
int i,j,k,row_offset,nlocal; 
int *local_a,*local_b,*px,*product_matrix; 
int p,rank,n,sender; 
int ROW=0,COL=1; 
int npes,dims[2],periods[2],keep_dims[2]; 
int other_rank,coords[2]; 
int last_column_coords; 
int my2drank,mycoords[2]; 
int dim_size; 
MPI_Comm comm_2d,comm_row,comm_col; 
MPI_Status status; 
double stime, ttime, etime;

MPI_Init(&argc,&argv); 
MPI_Comm_size(MPI_COMM_WORLD,&p); 
MPI_Comm_rank(MPI_COMM_WORLD,&rank); 

n = 16; 

 if (rank==0){

    stime=MPI_Wtime();

}

nlocal = n/sqrt(p); 
dim_size = sqrt(p); 

local_a = (int *)malloc((nlocal*nlocal)*sizeof(int)); 
local_b = (int *)malloc((nlocal)*sizeof(int)); 
px = (int *)malloc((nlocal)*sizeof(int)); 
product_matrix = (int *)malloc((nlocal)*sizeof(int)); 

/*Compute the size of square grid*/ 
dims[ROW] = dims[COL] = sqrt(p); 

/*Set up the catesian topology and get the rank and coordinates of the processin this topology*/ 
periods[ROW]=periods[COL]=1; /*set the periods for wrap-around connections*/ 

MPI_Cart_create(MPI_COMM_WORLD,2,dims,periods,1,&comm_2d); 
MPI_Comm_rank(comm_2d,&my2drank); /*get the rank in new topology*/ 
MPI_Cart_coords(comm_2d,my2drank,2,mycoords); /*get the coordinates*/ 

/*Create row-based topology*/ 
keep_dims[ROW] = 0; 
keep_dims[COL] = 1; 
MPI_Cart_sub(comm_2d,keep_dims,&comm_row); 

/*create column based topology*/ 
keep_dims[ROW] = 1; 
keep_dims[COL] = 0; 
MPI_Cart_sub(comm_2d,keep_dims,&comm_col); 

/*************Generate partial matrix ai at each process*******************/ 
printf("Generating Local Matrix A at process %d\n",rank); 

for(i=0;i <nlocal;i++)
{ 
for(j=0;j <nlocal;j++)
{ 
local_a[i*nlocal+j]=(mycoords[COL]*nlocal)+j+1; 
printf("%d\t",local_a[i*nlocal+j]); 
} 	
printf("\n"); 
} 
/**********************************************************************/ 



MPI_Barrier(comm_2d);

/****Generate partial vector b at the processes in the last column****/ 
last_column_coords = (dim_size - 1); 
if(mycoords[COL] == last_column_coords) 
{ 
printf("Generating Vector B at process %d\n",rank); 

for(i=0;i<nlocal;i++) 
{ 
local_b[i]=1; 
printf("%d\n",local_b[i]); 
} 
} 


MPI_Barrier(comm_2d);

/*Redistribute the vector b. */ 

/*the processors along the last column send their data to diagonal processors*/ 
if(mycoords[COL] == last_column_coords && mycoords[ROW] != last_column_coords) 
{ 
coords[ROW] = mycoords[ROW]; 
coords[COL] = mycoords[ROW]; 
MPI_Cart_rank(comm_2d,coords,&other_rank); 
printf("Distributing the partial vector B to Diagonal Process %d from Process %d\n",other_rank,rank); 
MPI_Send(local_b,nlocal,MPI_INT,other_rank,1,comm_2d); 
} 

MPI_Barrier(comm_2d);


if(mycoords[ROW] == mycoords[COL] && mycoords[ROW] != last_column_coords) 
{ 
coords[ROW]=mycoords[ROW]; 
coords[COL]= last_column_coords; 
MPI_Cart_rank(comm_2d,coords,&other_rank); 
printf("Receiving the partial vector B at Diagonal Process at %d from Process %d\n",rank,other_rank); 
MPI_Recv(local_b,nlocal,MPI_INT,other_rank,1,comm_2d,&status); 

} 

MPI_Barrier(comm_2d);


/*the diagonal processors perform a column wise broadcast*/ 
coords[0] = mycoords[COL]; 
MPI_Cart_rank(comm_col,coords,&other_rank); 
MPI_Bcast(local_b,nlocal,MPI_INT,other_rank,comm_col); 

MPI_Barrier(comm_2d);
MPI_Barrier(comm_col);


/*Main Computation Loop*/ 
for(i=0;i<nlocal;i++) 
{ 
px[i]=0; 
for(j=0;j<nlocal;j++) 
px[i] += local_a[i*nlocal+j]*local_b[j]; 
} 
MPI_Barrier(comm_2d);

/*perform the sum reduction along the rows to add up the partial dot-products and store the result in the last column of processes*/ 
coords[0] = last_column_coords; 
MPI_Cart_rank(comm_row,coords,&other_rank); 
MPI_Reduce(px,product_matrix,nlocal,MPI_INT,MPI_SUM,other_rank,comm_row); 
MPI_Barrier(comm_2d);
MPI_Barrier(comm_row);

if(mycoords[COL] == last_column_coords) 
{ 
printf("Product Vector at process %d\n",rank); 

for(i=0;i<nlocal;i++) 
{ 
printf("%d\n",product_matrix[i]); 
} 
} 

MPI_Barrier(comm_2d);
MPI_Comm_free(&comm_2d); 
MPI_Comm_free(&comm_row); 
MPI_Comm_free(&comm_col); 

free(px); 
free(local_a); 
free(local_b); 
free(product_matrix); 

if (rank==0)
  {
    etime = MPI_Wtime();
    ttime = etime - stime;

    printf("For n=%4d and %2d processors, Total time : %.10lf\n", n, p, ttime);

  }

MPI_Finalize(); 
}//main