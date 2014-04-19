#include <iostream>
#include <cstdlib>
#include <sys/time.h>
#include <stdint.h>

// Function Definition(s)
template <class any_data_type1> any_data_type1 *crand(int rows, int cols, any_data_type1 maxval);

// Function(s)
template <class any_data_type1>

any_data_type1 *crand(int rows, int cols, any_data_type1 maxval)
{
    struct timeval tv;
    
    time_t curtime;
    gettimeofday(&tv, NULL); 
    curtime=tv.tv_sec;
    
    //unsigned int iseed_date = (unsigned int) localtime(&curtime); // For 32-bit systems
    uintptr_t iseed_date = reinterpret_cast<uintptr_t>(localtime(&curtime)); // For 64-bit systems; might work on 32-bit ones too. PLEASE CHECK.

    unsigned long long int iseed = ((unsigned long long int) iseed_date*1000000000000) +((unsigned long long int) tv.tv_sec*1000000 + tv.tv_usec);
  
    srand (iseed);
    
	any_data_type1 *arr2D = new any_data_type1[rows*cols];
	for (int i=0; i<rows*cols; i++)
	arr2D[i] = maxval* ((double) rand ()/RAND_MAX); // RAND_MAX = 2147483647, irrespective of the datatype.

    return arr2D;
}


