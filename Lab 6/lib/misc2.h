
// Function Definitions
double *create_grid( int gridsize, int diag_num, double neigh_num);
void *setup_LShapedDomain(double *T, int n);

// About Function: Create an array to represent a tridiagonal matrix for a 2D domain.
//
// INPUTS:
// gridsize = Size of one side of the the 2D domain.
// diag_num = Scalar value for the diagonal elements
// neigh_num = Scalar for the neighbouring elements
//
// OUTPUTS:
// Double array of the 2D tridiagonal elements
double *create_grid( int gridsize, int diag_num, double neigh_num)
{
	int n=gridsize*gridsize;

	double *A= new double[n*n];	

	// Initialise A wiht zeros
	for(int i=0;i<n*n;i++)
		A[i] = 	0.0;

	// Set the diagonal elements of A
	for(int i=0;i<n*n;i=i+n+1)
		A[i] = 	diag_num;

	// Set the RIGHT elements of A
	for(int i=1;i<n*n;i=i+n+1)
		A[i] = neigh_num;

	// Set the LEFT elements of A
	for(int i=n;i<n*n;i=i+n+1)
		A[i] = neigh_num;

	// Set the UP elements of A
	for(int i=gridsize;i<n*n;i=i+n+1)
		A[i] = neigh_num;

	// Set the BOTTOM elements of A
	for(int i=n*gridsize;i<n*n;i=i+n+1)
		A[i] = neigh_num;

	return A;
}

// About Function: Reset the 2D domain to the requirements of the Problem 5 of ES611.
//
// INPUTS:
// T = The input array to be modified
// n = Number of elements of T
//
// OUTPUTS:
// T = The modified array
void *setup_LShapedDomain(double *T, int n)
{
	double bound_num = 0.0;
	double quad_num = 0.0;
	double source_num = 0.25;

	for(int i=0;i<n;i++)
		T[i] = 	bound_num;

	for(int i=0;i<n*n;i=i+n)
		T[i] = 	bound_num;
	
	for(int i=n-1;i<n*n;i=i+n)
		T[i] = 	bound_num;

	for(int i=(n-1)*n;i<n*n;i++)
		T[i] = 	bound_num;


	for(int i=n/2;i<n;i++)
		for(int j=n/2;j<n;j++)
			T[i*n+j] = quad_num;

	for(int i=n/8;i<(3*n)/8;i++)
		for(int j=n/8;j<(3*n)/8;j++)
			T[i*n+j] = source_num;

}

