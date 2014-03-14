#include <stdio.h>
#include "mpi.h"
#include<string.h>
#include<time.h>
#include<stdlib.h>

int main(int argc, char** argv)
{
	int my_rank;
	int p;
	int source;
	int dest;
	int tag=0;
	float sendMessage[1];
	float recMessage[1];
	MPI_Status status;
	
	/* Let the system do what it needs to start up MPI */
	MPI_Init(&argc, &argv);
	/* Get my process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	/* Find out how many processes are being used */
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	
	double start, end;
	// Check if number of processes is equal to 2
	if (p!=2)
	{
		if (my_rank==0)
			printf("The number of processes must be 2\n");
			exit(1);
	}
	else{
			// Start pingpong-ing between the processors
		int i;
		int pingpongTimes = 5;
	
		//sprintf(sendMessage, "%d", my_rank);
	
		for (i=0; i<pingpongTimes; i++)
		{	
			if (my_rank == 0)
			{
			start = MPI_Wtime();
			MPI_Send(sendMessage, 2, MPI_FLOAT, 1, tag, MPI_COMM_WORLD);
        	MPI_Recv(recMessage, 2, MPI_FLOAT, 1, tag, MPI_COMM_WORLD, &status);
        	end = MPI_Wtime();
			//printf("RTT1 = %.15f %ld\n", end-start, sizeof(int));
			
			}
			else if (my_rank == 1)
			{
			start = MPI_Wtime();
			MPI_Recv(recMessage, 2, MPI_FLOAT, 0, tag, MPI_COMM_WORLD, &status);
			MPI_Send(sendMessage, 2, MPI_FLOAT, 0, tag, MPI_COMM_WORLD);
			end = MPI_Wtime();
			//printf("RTT2 = %.15f\n", end-start);
			}		
		}
	}

	


	/* Shut down MPI */
	MPI_Finalize();
	
	return 0;

}
