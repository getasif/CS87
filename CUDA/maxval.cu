#include "book.h"
#include <climits>
#include <cstdlib>
#include <iostream>
#include "timer.h"

using namespace std;

#define imin(a,b) (a<b?a:b)

const int N = 16*1024*1024;

/* return a random floating point value in the range min to max */
float rand_in_range(float min, float max){
  return (max-min)*(1.0*(rand()%INT_MAX))/INT_MAX+min;
}

/* N is a global constant */
float max_cpu(float *a, int size){
  float mval = a[0];
  for(int i=1; i<size; i++){
    if (a[i] > mval){
      mval = a[i];
    }
  }
  return mval;
}

__global__ void max_gpu_single(float* dev_a, int size, float* dev_result ){
  float lmax=dev_a[0];
	for(int i=0; i<size; i++){
		if(dev_a[i] > lmax){
			lmax = dev_a[i];
		}
	}
  dev_result[0]=lmax;
}

int block_test(int numBlocks, float* dev_a, int size, float* dev_result){
	printf("Block kernel with %d blocks and 1 thread per block\n", numBlocks);
	//max_gpu_block<<<numBlocks,1>>>(dev_a,size,dev_result);
  return numBlocks;
}

int thread_test(int numThreads, float* dev_a, int size, float* dev_result){
	printf("Thread kernel with 1 block and %d threads per block\n", numThreads);
	//max_gpu_thread<<<1,numThreads>>>(dev_a,size,dev_result);
  return numThreads;
}

int combo_test(int numBlocks, int numThreads,
		float* dev_a, int size, float* dev_result){
	printf("Combo kernel with %d blocks and %d threads per block\n", 
			numBlocks, numThreads);
	//max_gpu_combined<<<numBlocks,numThreads>>>(dev_a,size,dev_result);
  return numBlocks;
}

void time_kernel(GPUTimer& gtime, float* dev_a, int size, float* dev_result,
		float* partial_result, int testID){

    int partial_size;
		gtime.start();

    // call the kernel
		switch (testID){
			case 1:
        partial_size=1;
        max_gpu_single<<<1,1>>>(dev_a,size,dev_result);
				break;
		  default:
				break;
		}

		gtime.stop();
		gtime.print();

    // copy the array 'result' back from the GPU to the CPU
    HANDLE_ERROR( cudaMemcpy( partial_result, dev_result,
                              partial_size*sizeof(float),
                              cudaMemcpyDeviceToHost ) );

    CPUTimer t;
    t.start();
    
    // finish up on the CPU side
    float ans = partial_result[0];
		for (int i=0; i<partial_size; i++) {
			if(partial_result[i] > ans){
				ans = partial_result[i];
			}
		}

    cout << "Max (by GPU) " << ans << endl;
    t.stop();
    printf("Time to run CPU-GPU finishing: %7.2f ms\n", 1000*t.elapsed());
		printf("\n");

}

int main( void ) {
    float   *a, *partial_result;
    float   *dev_a, *dev_result;
   
    GPUTimer gtime;

    //MAX size of partial results buffer
    int partial_size = 2048;

    // allocate memory on the cpu side
    a = (float*)malloc( N*sizeof(float) );
    partial_result = (float*)malloc( partial_size*sizeof(float) );

    // allocate the memory on the GPU
    HANDLE_ERROR( cudaMalloc( (void**)&dev_a,
                              N*sizeof(float) ) );
    HANDLE_ERROR( cudaMalloc( (void**)&dev_result,
                              partial_size*sizeof(float) ) );

    // fill in the host memory with data
    for (int i=0; i<N; i++) {
        a[i] = rand_in_range(0.0f,50000.0f);
        //cout << i << " " << a[i] << endl;
    }

    //initialize partial results to be all 0
    for(int i=0; i<partial_size; i++){
      partial_result[i]=0.0f;
    }

    // copy the arrays 'a' and 'b' to the GPU
    HANDLE_ERROR( cudaMemcpy( dev_a, a, N*sizeof(float),
                              cudaMemcpyHostToDevice ) );

    CPUTimer t;
    t.start();
    cout << "Max (by CPU) " << max_cpu(a, N) << endl;
    t.stop();
    printf("Time to run on CPU: %7.2f ms\n\n", 1000*t.elapsed());

		for( int t=1; t<2; t++){
		  time_kernel(gtime, dev_a, N, dev_result, partial_result, t);
    }

		// free memory on the gpu side
    HANDLE_ERROR( cudaFree( dev_a ) );
    HANDLE_ERROR( cudaFree( dev_result ) );

    // free memory on the cpu side
    free( a ); a=NULL;
    // free memory on the cpu side
    free( partial_result ); partial_result=NULL;
}
