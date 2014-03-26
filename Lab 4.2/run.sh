mpicc Sparse_Matrix_Vector_MPI_1.c -std=c99 -lm -lmpi
mpirun -np 4 ./a.out
