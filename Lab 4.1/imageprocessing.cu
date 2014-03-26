#include <iostream>
#include "cuda.h"
#include "cuda_runtime.h"
#include <fstream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;
/*Kernel Function. Please Note that if you follow the code given below,
the number of threads and pixel are equal, i.e. each thread corresponds to pixels*/
__global__ void imageprocessing(int *pixeldata,int *pixelupdate, int height, int width)
{
	int offset = (blockIdx.x*gridDim.x+blockIdx.y*gridDim.y+threadIdx.x);
	pixelupdate[offset]=(int) (0.21f*pixeldata[offset]+0.71f*pixeldata[offset+width*height]+0.07f*pixeldata[offset+2*width*height]);
}

int main(){
	int height=300, width=400; //Enter the image height and width. For a 640x480 image, int height=480, width=640; 
	int thperblk=width/4;//Enter the number of threads per block
	int i,j,k;
	int block_dim_x, block_dim_y, grid_dim_x, grid_dim_y;

	int *pixeldata, *pixelupdate;
	FILE *data;
	pixeldata = (int*) malloc(height*width*3*sizeof(int));
	
	/*The size of memory allocated will be height*width*sizeof(int) 
	for grayscale output data as there is only one value per pixel*/
	pixelupdate = (int*) malloc(height*width*3*sizeof(int));
	
	data = fopen("pixeldata.txt","r");
	printf("starting\n");
	for(k=0; k<3; k++){
		for(j=0; j<width; j++){
			for(i=0; i<height; i++){
				fscanf(data, "%d", &pixeldata[k*width*height+j*height+i] );
			}
		}
	}
	fclose(data);
	printf("done\n");
	
	int *pixeldata_device;
	int *pixelupdate_device;
	int size = height*width*3*sizeof(int);
	
	cudaMalloc((void**)&pixeldata_device, size);
	
	/*The size of memory allocated will be size/3 for grayscale 
	output data as there is only one value per pixel.
	Take care of this case in cudaMemcpy as well.*/
	cudaMalloc((void**)&pixelupdate_device, size); 
	
	block_dim_x=thperblk; //Enter the number of threads in the x-direction in each block
	block_dim_y=thperblk/block_dim_x;
	grid_dim_x=height/block_dim_x;
	grid_dim_y=width/block_dim_y;
	dim3 threads(block_dim_x, block_dim_y, 1);
	dim3 blocks(grid_dim_x, grid_dim_y, 1);
	
	//Take care of the changes in size of array for grayscale case
	cudaMemcpy(&pixeldata_device[0],&pixeldata[0],size,cudaMemcpyHostToDevice);
	cudaMemcpy(&pixelupdate_device[0],&pixeldata_device[0],size,cudaMemcpyDeviceToDevice);
	imageprocessing <<<blocks, threads>>> (pixeldata_device,pixelupdate_device,height,width);
	cudaMemcpy(&pixelupdate[0],&pixelupdate_device[0],size,cudaMemcpyDeviceToHost);
	
	FILE *output;
	printf("open\n");
	output = fopen("pixeldataout.txt","w");
	
	//Outermost 'k' loop will not be present for gray scale case
	for(k=0; k<3; k++){
		for(i=0; i<width; i++){
			for(j=0; j<height; j++){
				fprintf(output, "%d\t", pixelupdate[i*height+j] );
			}
		}	
	}
	fclose(output);
	printf("close\n");
	cudaFree(pixeldata_device);
	cudaFree(pixelupdate_device);
	free(pixeldata);
	free(pixelupdate);
	return 0;
	}