#include "lib/misc.h"
#include <sys/time.h>

int main(int argc,char **argv)
{
	// Initialize PetSc
	PetscInitialize(&argc,&argv,(char*)0,"Testing a program!");
	struct timeval start, stop;
	gettimeofday(&start,NULL);
	// Define params to the set of equations
	int num_rows;
	int num_cols;
	num_rows = 40;
	num_cols = 40;
	if (argv[1]!=NULL)
	{
		num_rows = atoi(argv[1]);
		num_cols = atoi(argv[1]);
	}

	double *arr1 = crand(num_rows,num_cols,1.0);
	double *arr2 = crand(1,num_rows,1.0);

	double *arr3 = solvelin(arr1,arr2,num_rows,num_cols);
	//print(arr3,1,num_rows,"arr3");

	gettimeofday(&stop,NULL);
    printf("%lu,", stop.tv_usec-start.tv_usec);

	// Close out
	PetscFinalize();

	return 0;
}
