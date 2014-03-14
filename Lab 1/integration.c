/* Parallel Trapezoidal Rule
 *
 * Author: Dhruv Pancholi
 *
 * Output: Estimation of the integral from a to b of f(x)
 *         using the trapezoidal rule and n trapezoids.
 *
 * Algorithm:
 *    1.  Each process calculates "its" interval of
 *        integration.
 *    2.  Each process estimates the integral of f(x)
 *        over its interval using the trapezoidal rule.
 *    3a. Each process != 0 sends its integral to 0.
 *    3b. Process 0 sums the calculations received from
 *        the individual processes and prints the result.
 *
 * Notes:  
 *    1.  f(x), a, b, n are all hardwired.
 *    2.  The number of processes (p) should divide evenly by
 *        the number of trapezoids (n = 1024)
 */

#include <stdio.h>
#include <math.h>
#include "mpi.h"
#include "mpe.h"
#define f(x) log(x + 1)/(x*x + 4*x + 3)

/* Calculate local integral  */
double Trap(float local_a, float local_b, long int local_n, double h);

int main(int argc, char** argv)
{
  int         my_rank;          /* My process rank           */
  int         p;                /* The number of processes   */
  float       a = 0.0;          /* Left endpoint             */
  float       b = 30.0;         /* Right endpoint            */
  long int    n = 10000000;     /* Number of trapezoids      */
  double      h;                /* Trapezoid base length     */
  float       local_a;          /* Left endpoint my process  */
  float       local_b;          /* Right endpoint my process */
  long int    local_n;          /* Number of trapezoids for  */
                                /* my calculation            */
  double      integral;         /* Integral over my interval */
  double      total_integral;   /* Total integral            */
  int         source;           /* Process sending integral  */
  int         dest = 0;         /* All messages go to 0      */
  int         tag = 0;
  MPI_Status  status;
  double startTime, endTime, timeDifference;
  char jumpShotFile[10];

  /* Let the system do what it needs to start up MPI */
  MPI_Init(&argc, &argv);
  /* Get my process rank */
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  /* Find out how many processes are being used */
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  // Initializing the log, after communication
  MPE_Init_log();
  int event1a = MPE_Log_get_event_number(); 
  int event1b = MPE_Log_get_event_number();
  int event2a = MPE_Log_get_event_number(); 
  int event2b = MPE_Log_get_event_number();
  int event3a = MPE_Log_get_event_number(); 
  int event3b = MPE_Log_get_event_number();
  MPE_Describe_state(event1a, event1b, "Receive", "blue");
  MPE_Describe_state(event2a, event2b, "Send", "yellow");
  MPE_Describe_state(event3a, event3b, "Compute", "red");
  // Starting to log
  MPE_Start_log();

 
  //sprintf(jumpShotFile, "integral%d", p);

  if (my_rank==0)
          startTime=MPI_Wtime();

  h = (b - a) / n;    /* h is the same for all processes */
  local_n = n / p;    /* So is the number of trapezoids */

  /* Length of each process' interval of integration = local_n * h.
   * So my interval starts at: */
  MPE_Log_event(event3a, 0, "start compute");
  local_a = a + my_rank * local_n * h;
  local_b = local_a + local_n * h;
  integral = Trap(local_a, local_b, local_n, h);
  MPE_Log_event(event3b, 0, "end compute");

  /* Add up the integrals calculated by each process */
  if (my_rank == 0)
  {
    total_integral = integral;
    for (source=1; source<p; source++)
    {
      MPE_Log_event(event1a, 0, "Start to receive");
      MPI_Recv(&integral, 1, MPI_DOUBLE, source, tag, MPI_COMM_WORLD, &status);
      MPE_Log_event(event1b, 0, "Recieved");
      total_integral = total_integral + integral;
    }
  }
  else{
    MPE_Log_event(event2a, 0, "Start to Send");
    MPI_Send(&integral, 1, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);
    MPE_Log_event(event2b, 0, "Sent");
  }

  if (my_rank==0)
  {
    endTime = MPI_Wtime();
    timeDifference = endTime - startTime;         
  }

  /* Print the result */
  if(my_rank == 0)
  {
    printf("With n = %ld trapezoids, \n", n);
    printf("our estimate of the integral from %f to %f = %e\n", a, b, total_integral);
    printf("Time taken for whole computation = %f seconds\n", timeDifference);
  }

  // Before finalize
  MPE_Finish_log(argv[1]);
  /* Shut down MPI */
  MPI_Finalize();
  
  return 0;
} /* main */

double Trap(float    local_a   /* in */,
            float    local_b   /* in */,
            long int local_n   /* in */,
            double   h         /* in */)
{
  double integral;   /* Store result in integral  */
  double x;
  long int i;

  integral = (f(local_a) + f(local_b))/2.0;
  x = local_a;
  for(i=1; i<=local_n-1; i++)
  {
    x = x + h;
    integral = integral + f(x);
  }
  integral = integral*h;

  return integral;
} /* Trap */