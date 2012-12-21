// a simple example of how to use myopengllib library to 
// animate a cuda computation
// Most of the openGL-Cuda interoperability stuff is hidden
// in the GPUDisplayData class.  However, you need to think
// a little bit about writing an event driven program since
// you register animation and clean-up events with this libary
// and then run your animation.
//
// This example application doesn't do anything recognizable,
// but shows how to use the GPUDisplayData library and how to
// write animate and clean_up functions to pass to the  
// GPUDisplayData.AnimateComputation  method.
//
// (newhall, 2011)

#include "myopengllib.h"


#define DIM 300

static void animate_simple(uchar4 *disp, void *mycudadata);
static void clean_up(void);
__global__ void int_to_color( uchar4 *optr, const int *my_cuda_data );
__global__ void  simplekernel(int *data, int* b); 


// just use a global to avoid huge stack space usage
// of a local (mallocing up heap space would be fine too)
static int matrix[DIM][DIM];

// if your program needs more GPU data, use a struct
// with fields for each value needed.
typedef struct my_cuda_data {
  int *dev_grid;
  int *bogus;
  int start;
  float end;

} my_cuda_data;

// single var holds all program data.  This will be passed
// to the GPUDisplayData constructor 
my_cuda_data simple_prog_data;

int main(int argc, char *argv[])  {


  // The call to the constructor has to come before any calls to 
  // cudaMalloc or other Cuda routines
  // This is part of the reason why we are passing the address of 
  // a struct with fields which are ptrs to cudaMalloc'ed space
  // The other reason is that adding a level of interection 
  // is the answer to every problem.
  GPUDisplayData my_display(DIM, DIM, &simple_prog_data, "Simple openGL-Cuda");

  // initialize application data 
  for(int i =0; i < DIM; i++) {
    for(int j =0; j < DIM; j++) {
      matrix[i][j] = j;
    }
  }

  // allocate memory space for our application data on the GPU
  HANDLE_ERROR(cudaMalloc((void**)&simple_prog_data.dev_grid, 
        sizeof(int)*DIM*DIM), "malloc dev_grid") ;

  // copy the initial data to the GPU
  HANDLE_ERROR (cudaMemcpy(simple_prog_data.dev_grid, matrix, 
        sizeof(int)*DIM*DIM, cudaMemcpyHostToDevice), "copy dev_grid to GPU") ;

  HANDLE_ERROR(cudaMalloc((void**)&simple_prog_data.bogus, 
        sizeof(int)*DIM*DIM), "malloc dev_grid") ;


  // register a clean-up function on exit that will call cudaFree 
  // on any cudaMalloc'ed space
  my_display.RegisterExitFunction(clean_up); 

  // have the library run our Cuda animation
  my_display.AnimateComputation(animate_simple);

  return 0;
}

// cleanup function passed to AnimateComputatin method.
// it is called when the program exits and should clean up
// all cudaMalloc'ed state.
// Your clean-up function's prototype must match this, which is 
// why simple_prog_data needs to be a global
static void clean_up(void) {
  cudaFree(simple_prog_data.dev_grid);
  cudaFree(simple_prog_data.bogus);
}

// amimate function passed to AnimateComputation: 
// this function will be called by openGL's dislplay function.
// It can contain code that runs on the CPU and also calls to
// to CUDA kernel code to do a computation and to change the
// display the results using openGL...you need to change the
// display color values based on the application values
// 
// devPtr: is pointer into openGL buffer of rgba values (but
//         the field names are x,y,z,w
// my_data: is pointer to our cuda data that we passed into the 
//          constructor
// 
// your animate function prototype must match this one:
static void animate_simple(uchar4 *devPtr, void *my_data) {

  my_cuda_data *simple_data = (my_cuda_data *)my_data;
  dim3 blocks(DIM, DIM); 

  // comment out the for loop to do a display update every 
  // execution of simplekernel
  //for(int i=0; i < 90; i++) 
    simplekernel<<<blocks,1>>>( simple_data->dev_grid, simple_data->bogus); 

  int_to_color<<<blocks,1>>>(devPtr, simple_data->dev_grid); 
}

// a kernel to set the color the opengGL display object based 
// on the cuda data value
//  
//  optr: is an array of openGL RGB pixels, each is a 
//        4-tuple (x:red, y:green, z:blue, w:opacity) 
//  my_cuda_data: is cuda 2D array of ints
__global__ void int_to_color( uchar4 *optr, const int *my_cuda_data ) {

    // get this thread's block position to map into
    // location in opt and my_cuda_data
    // the row and col values depend on how you parallelize the
    // kernel (<<<blocks, threads>>>).  This is how to do it for
    // a DIMxDIM grid of blocks, each block with a single thread
    // If each block has more than one thread, or blocks of threads
    // then you need to do something more complicated to get a 
    // thread's mapping to a row and col value
    int row = blockIdx.x;  
    int col = blockIdx.y;
    int offset = col + row * gridDim.x;

    if(col < DIM && row < DIM) {
      // change this pixel's color value based on some strange
      // functions of the my_cuda_data value
      optr[offset].x = (my_cuda_data[offset]+10)%255;  // R value
      optr[offset].y = (my_cuda_data[offset]+100)%255; // G value
      optr[offset].z = (my_cuda_data[offset]+200)%255; // B value
      optr[offset].w = 255;  // just set this to 255 always
    }
}

// a simple cuda kernel: cyclicly increases a points value by 10
//  data: a "2D" array of int values
__global__ void  simplekernel(int *data, int* b) {

    int row = blockIdx.x;
    int col = blockIdx.y;
    int offset = col + row * gridDim.y;

    if(col < DIM && row < DIM) {
      data[offset] = (data[offset]+10)%1000;
    }
    b[offset] = 1;




}


