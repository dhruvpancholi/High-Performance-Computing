/*Name: Dhruv Pancholi
* Roll No.: 11110028
* Julia Set on CUDA
*/
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "stdlib.h"
#include "cuda.h"
#include "cuda_runtime.h"
#include<iostream>

#define DEBUG 0

typedef struct complextype
        {
        float real, imag;
        } Compl;

void compute(int X_RESN, int Y_RESN, int max_iteration);

/*This is the main routine which computes the color for each pixel in the screen.
The color is given in the variable <TheK>, which is obtained by iterating the
complex sequence Z = Z^2 + C 
*/
__global__ void iterate(int maxc,int minc, int X_RESN, int Y_RESN, int* TheK, int max_iteration)
{
	int i,j;
	int k, imax = max_iteration;
	float lengthsq, temp;
	Compl z,c;

	i = blockIdx.x*blockDim.x+threadIdx.x;
	j = blockIdx.y*blockDim.y+threadIdx.y;

	if (i<X_RESN && j<Y_RESN)
	{
		c.real = -0.8f; c.imag = 0.156f;
		z.real   = -2.0f + (4.0f/ (X_RESN-1) )*(float)j ;
		z.imag   = -2.0f - (4.0f/ (Y_RESN-1) )*( (float)i - (Y_RESN - 1) );
	}
	else{return;}
	

	k = 0;

	do  {       //iterate for pixel color 

	temp = z.real*z.real - z.imag*z.imag + c.real;
	z.imag = 2.0f*z.real*z.imag + c.imag;
	z.real = temp;
	lengthsq = z.real*z.real+z.imag*z.imag;
	k++;

	} while (lengthsq < 4.0f && k < imax);

	TheK[i*Y_RESN+j]=minc + ((float)(maxc - minc)/(imax -1 ))*( k -1 );

} // End of iterate


int main(int argc, char const *argv[]){
int max_iteration=200;
if (argv[1]!=NULL){
	max_iteration=atoi(argv[1]);
}
  if ( DEBUG  )
    printf("1. Entering code\n");

        int rank;
        int nproc;
        int             X_RESN = 800;
        int             Y_RESN = 800;
  
  if ( DEBUG  )
    printf("2. Initialized variables\n");
    
	clock_t t1, t2;
        t1=clock();
           compute(X_RESN,Y_RESN, max_iteration);
	t2=clock();   
  
  if ( DEBUG  ){printf("15. Finished Computation\n");}
    
	printf("Total Time Taken = %f Seconds\n",((double)( t2 - t1 )/ (double) CLOCKS_PER_SEC ) ); 

return 0;
}

void compute(int X_RESN, int Y_RESN, int max_iteration)
{
        Window          win;                            /* initialization for a window */
        unsigned
        int             width, height,                  /* window size */
                        x, y,                           /* window position */
                        border_width,                   /*border width in pixels */
                        display_width, display_height,  /* size of screen */
                        screen;                         /* which screen */

        char            *window_name = "Julia Set", *display_name = NULL;
        GC              gc;
        unsigned long   valuemask = 0;
        XGCValues       values;
        Display         *display;
        XSizeHints      size_hints;
        Pixmap          bitmap;
        XPoint          points[800];
        FILE            *fp, *fopen ();
        char            str[100];
        Status          rc;
        int             tmp=1;
        XSetWindowAttributes attr[1];
       
       if ( DEBUG  )
         printf("3. Finished Declaring X-Window parameters\n ");

       /* Important variables */
        int i, j, k;
        size_t size = (X_RESN*Y_RESN)*sizeof(int);
        int *TheK = (int *)malloc(size);
        int *TheK_device;
        cudaMalloc((void **) &TheK_device, size);
       
        /* connect to Xserver */
        if ( DEBUG  )
         printf("4. Connecting to X-Server\n ");
	 
        if (  (display = XOpenDisplay (display_name)) == NULL ) {
           fprintf (stderr, "drawon: cannot connect to X server %s\n",
                                XDisplayName (display_name) );
          exit (-1);
        }
        
        /* get screen size */
        /*screen variable stands for which screen to be used*/
        screen = DefaultScreen (display);
        display_width = DisplayWidth (display, screen);
        display_height = DisplayHeight (display, screen);

        /* set window size */

        width = X_RESN;
        height = Y_RESN;

        /* set window position */

        x = 0;
        y = 0;

        /* create opaque window */
        if ( DEBUG  )
         printf("5. Creating an X-Window\n");
	  
        border_width = 4;
        win = XCreateSimpleWindow (display, RootWindow (display, screen),
                                x, y, width, height, border_width,
                                BlackPixel (display, screen), WhitePixel (display, screen));

        size_hints.flags = USPosition|USSize;
        size_hints.x = x;
        size_hints.y = y;
        size_hints.width = width;
        size_hints.height = height;
        size_hints.min_width = 300;
        size_hints.min_height = 300;

        XSetNormalHints (display, win, &size_hints);
        XStoreName(display, win, window_name);

        /* create graphics context */

        gc = XCreateGC (display, win, valuemask, &values);

        

         /* allocate the set of colors we will want to use for the drawing. */
        if ( DEBUG  )
         printf("6. Setting Display Attributes\n ");

        XSetBackground (display, gc, WhitePixel (display, screen));
        XSetForeground (display, gc, BlackPixel (display, screen));
        XSetLineAttributes (display, gc, 1, LineSolid, CapRound, JoinRound);

        

        attr[0].backing_store = Always;
        attr[0].backing_planes = 1;
        attr[0].backing_pixel = BlackPixel(display, screen);

        XChangeWindowAttributes(display, win, CWBackingStore | CWBackingPlanes | CWBackingPixel, attr);

        XMapWindow (display, win);
        XSync(display, 0);

        

        int white, black;
        int generic,process;
        int minc, maxc;

        if ( DEBUG  )
         printf("7. Computing the Numerical Value of Black and White Pixel\n");

        white = WhitePixel (display, screen);       /* color value for white */
        black = BlackPixel (display, screen);       /* color value for black */
        
        minc = (white > black) ? black : white;
        maxc = (white > black) ? white : black;
        
	/* Loop around each point and determine the color*/
	
	if ( DEBUG  )
         printf("8. Entering the Iterate function to compute the Color at each point\n");
	
	dim3 blocks(80,80,1);
	dim3 threads(10,10,1);
	iterate<<<blocks,threads>>>(maxc, minc, X_RESN, Y_RESN,TheK_device, max_iteration);
	cudaDeviceSynchronize();

	if ( DEBUG  )
         printf("9. End of Iterate() Function.. exiting\n");

    cudaError_t error;
    error= cudaMemcpy(TheK, TheK_device, size, cudaMemcpyDeviceToHost);
    if (DEBUG)
    {
    	printf("\nError in Copying: %s\n", cudaGetErrorString(error));
    }
        
	if (DEBUG )
	{
	  printf("10. Value for white = %d\n",white);
	  printf("11. Value for black = %d\n",black);
	}  
	
    
    	clock_t t1, t2;
        t1=clock();
           
	if ( DEBUG  )
         printf("12. Loop Around each Pixel, Set the Color and Plot the Point\n ");
    
       for(i = 0; i < Y_RESN ; i++)
        {
                for(j=0; j < X_RESN; j++)
                 {
                        XSetForeground(display, gc, TheK[i*Y_RESN+j]  );
                        XDrawPoint (display, win, gc, j, i);
                 }

        }
	t2=clock();   
       
        if ( DEBUG  )
         printf("13. End of Code\n ");
       
    if (DEBUG)
      {
      	printf("Total Time Taken for Plotting= %f Seconds\n",((double)( t2 - t1 )/ (double) CLOCKS_PER_SEC ) );
      }
        sleep(5);
        XFlush (display);
        
	if ( DEBUG  )
         printf("14. Free the Assigned Memory\n");
	
	free(TheK);
	cudaFree(TheK_device);

}
