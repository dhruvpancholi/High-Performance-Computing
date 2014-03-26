/* serial_mat_mult.c -- multiply two square matrices on a single processor
 *
 * Input: 
 *     n: order of the matrices
 *     A,B: factor matrices
 *
 * Output:
 *     C: product matrix
 */
#include <stdio.h>
#include "mpi.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define MAX_ORDER 10

typedef float MATRIX_T[MAX_ORDER][MAX_ORDER];

int main(int argc, char** argv)
{
    int my_rank;
    int p;
    int source;
    int dest;
    int tag=0;
    char sendMessage[1];
    char recMessage[1];
    MPI_Status status;
    
    /* Let the system do what it needs to start up MPI */
    MPI_Init(&argc, &argv);
    /* Get my process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    /* Find out how many processes are being used */
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    
    double start, end;
    int n;
    MATRIX_T  A;
    MATRIX_T  B;
    MATRIX_T  C;

    void Read_matrix(char* prompt, MATRIX_T A, int n);
    void Serial_matrix_mult(MATRIX_T A, MATRIX_T B, MATRIX_T C, int n);
    void Print_matrix(char* title, MATRIX_T C, int n);
    if (my_rank == 0)
    {
        start = MPI_Wtime();

/*      MPI_Send(sendMessage, 2, MPI_CHAR, 1, tag, MPI_COMM_WORLD);
        MPI_Recv(recMessage, 2, MPI_CHAR, 1, tag, MPI_COMM_WORLD, &status);*/
        printf("What's the order of the matrices?\n");
        scanf("%d", &n);
        Read_matrix("Enter A", A, n);
        Print_matrix("A = ", A, n);
        Read_matrix("Enter B", B, n);
        Print_matrix("B = ", B, n);
        Serial_matrix_mult(A, B, C, n);
        Print_matrix("Their product is", C, n);

        end = MPI_Wtime();
    }

    /* Shut down MPI */
    MPI_Finalize();
    
    return 0;
    
}  /* main */


/*****************************************************************/
void Read_matrix(
         char*     prompt  /* in  */,
         MATRIX_T  A       /* out */,
         int       n       /* in  */) {
    int i, j;

    printf("%s\n", prompt);
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            A[i][j]=rand(&i);
    printf("Read Complete!\n");
}  /* Read_matrix */


/*****************************************************************/
/* MATRIX_T is a two-dimensional array of floats */
void Serial_matrix_mult(
        MATRIX_T   A   /* in  */,
        MATRIX_T   B   /* in  */,
        MATRIX_T   C   /* out */,
        int        n   /* in  */) {

    int i, j, k;

    void Print_matrix(char* title, MATRIX_T C, int n);

    Print_matrix("In Serial_matrix_mult A = ", A, n);
    Print_matrix("In Serial_matrix_mult B = ", B, n);

    for (i = 0; i < n; i++)    
        for (j = 0; j < n; j++) {
            C[i][j] = 0.0;
            for (k = 0; k < n; k++)
                C[i][j] = C[i][j] + A[i][k]*B[k][j];
            printf("i = %d, j = %d, c_ij = %f\n", i, j, C[i][j]);
        }
}  /* Serial_matrix_mult */


/*****************************************************************/
void Print_matrix(
         char*     title  /* in  */,
         MATRIX_T  C       /* out */,
         int       n       /* in  */) {
    int i, j;

    printf("%s\n", title);
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++)
            printf("%4.1f ", C[i][j]);
        printf("\n");
    }
}  /* Read_matrix */
