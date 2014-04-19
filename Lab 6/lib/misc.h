#include <petscvec.h>
#include <petscmat.h>
#include <petscksp.h>

#include "RandTemplate.h"
#include "PrintTemplate.h"


// ****************************************************************************************************************************************
// Function Definitions

// 1. Convert array to PetSc array
PetscScalar *Arr2PetscArr(double *arr2, int SIZE);

// 2. Convert PetSc array to vector
Vec PetscArr2Vec(PetscScalar *array1, int N);

// 3. Convert array to PetSc Mat
Mat Arr2Mat(double *arr10, int num_rows, int num_cols);

// 4. Print PetSc matrix
void printMat(Mat A);

// 5. Print PetSc vector
void printVec(Vec x);

// 6. Array to Vec
Vec Arr2Vec(double *arr2, int SIZE);

// 7. Vec to Array
double *Vec2Arr(Vec x, int SIZE);

// 8. Solve linearv equations
double *solvelin(double *arr1, double *arr2, int num_rows, int num_cols);

// ****************************************************************************************************************************************




// ****************************************************************************************************************************************
// Actual Functions
// ****************************************************************************************************************************************

// About Function: Create a Petsc vector from a Petsc array
//
// INPUTS:
// array1 = Petsc array to be converted to petsc vector
// N = Number of elements in array1
//
// OUTPUTS:
// Petsc vector with the same elements as in array1
Vec PetscArr2Vec(PetscScalar *array1, int N)
{  
  // Setup vector
  Vec x;
  VecCreate(PETSC_COMM_WORLD,&x);
  VecSetSizes(x,PETSC_DECIDE,N);
  VecSetFromOptions(x);

  // Place PetSc array as Vector
  VecPlaceArray(x,array1);

  return x;
}

// ****************************************************************************************************************************************

// About Function: Create a Petsc array from a regular array
//
// INPUTS:
// arr2 = Regular array to be converted to petsc array
// SIZE = Number of elements in arr2
//
// OUTPUTS:
// Petsc array with the same elements as in arr2
PetscScalar *Arr2PetscArr(double *arr2, int SIZE)    // Transfer data from normal array to PetSc array
{
  PetscScalar *array1;
  PetscMalloc(SIZE*sizeof(PetscScalar),&array1);
   
  for(int i=0;i<SIZE;i++)
  	array1[i]=arr2[i];

  return array1;
}

// ****************************************************************************************************************************************

// About Function: Create a petsc mat from a regular array
//
// INPUTS:
// arr10 = Regular array to be converted to petsc mat
// SIZE = Number of elements in arr2
//
// OUTPUTS:
// Petsc mat with the same elements as in arr10
Mat Arr2Mat(double *arr10, int num_rows, int num_cols)
{
  Mat A;

  MatCreate(PETSC_COMM_WORLD,&A);
  MatSetSizes(A,num_rows,num_cols,num_rows,num_cols);
  MatSetFromOptions(A);
  MatMPIAIJSetPreallocation(A,num_rows,NULL,num_cols,NULL);
  MatSeqAIJSetPreallocation(A,num_rows*num_cols,NULL);

  PetscInt Ii = 0;
  PetscInt J = 0;
  PetscScalar v;
  
  for(int count1=0;count1<num_rows;count1++)
  {
	J = 0;
	for(int count2=0;count2<num_cols;count2++)
  	{
		  v = arr10[count1*num_cols+count2];
		  MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);
		  J = J+1;
	}
  	Ii = Ii + 1;
  }
  return A;
}
// ****************************************************************************************************************************************

// About Function: Print petsc mat elements
void printMat(Mat A)
{
	  MatAssemblyBegin(A,MAT_FINAL_ASSEMBLY);
	  MatAssemblyEnd(A,MAT_FINAL_ASSEMBLY);
	  //MatView(A,PETSC_VIEWER_STDOUT_WORLD);
}

// ****************************************************************************************************************************************

// About Function: Print petsc vec elements
void printVec(Vec x)
{
	VecView(x,PETSC_VIEWER_STDOUT_WORLD);	
}

// ****************************************************************************************************************************************

// About Function: Create a petsc vec from a regular array
//
// INPUTS:
// arr2 = Regular array to be converted to petsc vector
// SIZE = Number of elements in arr2
//
// OUTPUTS:
// Petsc vec with the same elements as in arr2
Vec Arr2Vec(double *arr2, int SIZE)
{
  PetscScalar *array1;
  PetscMalloc(SIZE*sizeof(PetscScalar),&array1);
   
  for(int i=0;i<SIZE;i++)
  	array1[i]=arr2[i];

    // Setup vector
  Vec x;
  VecCreate(PETSC_COMM_WORLD,&x);
  VecSetSizes(x,PETSC_DECIDE,SIZE);
  VecSetFromOptions(x);

  // Place PetSc array as Vector
  VecPlaceArray(x,array1);

  return x;

}

// ****************************************************************************************************************************************

// About Function: Create a regular array from a petsc vec
//
// INPUTS:
// Vec x = Petsc vec to be converted to a regular array
// SIZE = Number of elements in vec x
//
// OUTPUTS:
// Regular array with the same elements as in vec x
double *Vec2Arr(Vec x, int SIZE)
{
	double *arr = (double *)malloc(SIZE*sizeof(double));
	PetscScalar v;

	PetscInt Ii = 0;	
	for (int i=0; i<SIZE; i++)
	{
		VecGetValues(x,1,&Ii,&v);
		Ii = Ii+1;
		arr[i] = v;
	}

	return arr;

}

// About Function: Solve AX=B 
//
// INPUTS:
// arr1 = The A part
// arr2 = The B part
// num_rows = Number of rows in A
// num_cols = Number of rows in B
//
// OUTPUTS:
// arr3 = The solved x part
double *solvelin(double *arr1, double *arr2, int num_rows, int num_cols)
{
  Mat A = Arr2Mat(arr1,num_rows,num_cols);
  Vec b = Arr2Vec(arr2, num_rows);

  Vec x;
  VecCreate(PETSC_COMM_WORLD,&x);
  VecSetSizes(x,PETSC_DECIDE,num_rows);
  VecSetFromOptions(x);

  KSP ksp;
  KSPCreate(PETSC_COMM_WORLD,&ksp);
  KSPSetOperators(ksp,A,A,DIFFERENT_NONZERO_PATTERN);
  KSPSetType(ksp,KSPCG);
  KSPSetFromOptions(ksp);

  printMat(A);
  //printVec(b);

  KSPSolve(ksp,b,x);
  //printVec(x);

  double *arr3 = Vec2Arr(x,num_rows);

  VecDestroy(&b);
  VecDestroy(&x);

  return arr3;

}

