echo "This is the implementation when program itself initializes the values of the matrix"
mpicc Matrix_Vector_hstrip.c -lm -lmpi -std=c99
mpirun -np 4 ./a.out 12
