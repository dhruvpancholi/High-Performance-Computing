/* Monte Carlo Simulation for the calculation of value of PI
 *
 * Output: The value of PI.
 *
 * Author: Dhruv Pancholi
 *
 */

#include <stdio.h>
#include <math.h>
#include "mpi.h"
#include "mpe.h"
#include "stdlib.h"

#ifndef NUM_NEEDLES
#define NUM_NEEDLES 100000000
#endif

#ifndef NUM_TRIALS
#define NUM_TRIALS 100
#endif

#define f(x) log(x + 1)/(x*x + 4*x + 3)

/* Calculate local integral  */
double Trap(float local_a, float local_b, long int local_n, double h);
float throwNeedles(int numNeedles);
double estPi(double precision, int numTrials);
double getEst(int numNeedles, int numTrials);
double mean(double arr[], int size);
double standardDeviation(double arr[], double mean, int numTrials);
double trapInt(double a, double b, int n);
void runtrapInt(int x);
double mathFunc(double x);
double adapQuad(double a, double b);

double estimates[NUM_TRIALS];

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
  long double      integral;         /* Integral over my interval */
  long double      total_integral;   /* Total integral            */
  int         source;           /* Process sending integral  */
  int         dest = 0;         /* All messages go to 0      */
  int         tag = 0;
  MPI_Status  status;
  double startTime, endTime, timeDifference;

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

  if (my_rank==0)
	  startTime=MPI_Wtime();

  local_n = n / p;    /* So is the number of trapezoids */

  /* Length of each process' interval of integration = local_n * h.
   * So my interval starts at: */
  MPE_Log_event(event3a, 0, "start compute");
  integral = throwNeedles(local_n);
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
  else {
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
    printf("The real value of PI is 3.141592653589793238462643\n");
    printf("our estimate of the value of PI is %.25Lf\n", total_integral/p);
    printf("Time taken for whole computation = %f seconds\n", timeDifference);
  }  
  // Before finalize
  MPE_Finish_log(argv[1]);
  /* Shut down MPI */
  MPI_Finalize();
  
  return 0;
} /* main */

float throwNeedles(int numNeedles){
	int inCircle=0;
	for (int i = 0; i < numNeedles; i++)
	{
		long double x=(double)rand()/RAND_MAX;
		long double y=(double)rand()/RAND_MAX;
		if (pow((long double)(x*x+y*y),0.5L)<=1.0)
		{
			inCircle++;
		}
	}
	return 4.0L*((long double)inCircle/(long double)(numNeedles));
}
double estPi(double precision, int numTrials){
	double numNeedles = NUM_NEEDLES;
	double sDev=precision;
	double result;
	while(sDev>=precision/2.0){
		result = getEst(numNeedles, numTrials);
		sDev = standardDeviation(estimates, mean(estimates, numTrials), numTrials);
		numNeedles*=2;
	}
	return result;
}
double getEst(int numNeedles, int numTrials){

	for (int i = 0; i < numTrials; i++)
	{
		estimates[i] = throwNeedles(numNeedles);
	}
	double curEst = mean(estimates, numTrials);
	return curEst;
}
double mean(double arr[], int size){
	float sum=0;
	for (int i = 0; i < size; i++)
	{
		sum+=arr[i];
	}
	return sum/size;
}
double standardDeviation(double arr[], double mean, int numTrials){
	double sum=0;
	for (int i = 0; i < numTrials; i++)
	{
		sum+=pow(arr[i]-mean,2);
	}
	return sqrt(sum/(double)pow((double)numTrials,2));
}

