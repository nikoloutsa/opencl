#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CL/cl.h>
#include "utils.h"
#include "ocl_macros.h"
#include "print_info_property.h"

//#define DEVICE_TYPE CL_DEVICE_TYPE_GPU
#define DEVICE_TYPE CL_DEVICE_TYPE_ALL
//#define DEVICE_TYPE CL_DEVICE_TYPE_CPU
//#define DEVICE_TYPE CL_DEVICE_TYPE_ACCELERATOR
//#define DEVICE_TYPE CL_DEVICE_TYPE_DEFAULT

#define VECTOR_SIZE 128

static double get_second(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec / 1e6;
}

int main(int argc, char *argv[]) {

   int pidx,didx;

   /* If no parameter passed to program print error
	  message and exit */
   if (argc < 3)
   {
	  print_platforms_devices();
	  printf("==================================\n");
	  printf("Usage: opencl [platforms] [device]\n");
	  exit(-1); 
   }

   pidx = atoi(argv[1]); 
   didx = atoi(argv[2]);


   cl_context context;
   cl_command_queue command_queue;
   cl_int clStatus;

   create_context(pidx, didx, &context, &command_queue, 0);

  // --------------------------------------------------------------------------
  // load kernels 
  // --------------------------------------------------------------------------
  char *src;
  src = read_source("my_kernel.cl");

  cl_kernel kernel = build_kernel(context, src, "my_kernel", NULL);

  double alpha = 2.0;
  // Allocate space for vectors A, B and C
  double *A = (double*)malloc(sizeof(double)*VECTOR_SIZE);
  double *B = (double*)malloc(sizeof(double)*VECTOR_SIZE);
  for(int i = 0; i < VECTOR_SIZE; i++)
  {
	 A[i] = -1.0;
	 B[i] = -1.0;
  }

  // Create memory buffers on the device for each vector
  cl_mem A_clmem = clCreateBuffer(context, CL_MEM_READ_WRITE,
		VECTOR_SIZE * sizeof(double), NULL, &clStatus);
  cl_mem B_clmem = clCreateBuffer(context, CL_MEM_READ_WRITE,
		VECTOR_SIZE * sizeof(double), NULL, &clStatus);

  // Copy the Buffer A and B to the device. We do a blocking write to the device buffer.
  clStatus = clEnqueueWriteBuffer(command_queue, A_clmem, CL_TRUE, 0,
		VECTOR_SIZE * sizeof(double), A, 0, NULL, NULL);
  OCL_LOG(clStatus, "clEnqueueWriteBuffer " );
  clStatus = clEnqueueWriteBuffer(command_queue, B_clmem, CL_TRUE, 0,
		VECTOR_SIZE * sizeof(double), B, 0, NULL, NULL);
  OCL_LOG(clStatus, "clEnqueueWriteBuffer " );

  // Set kernel arguments
  clStatus = clSetKernelArg(kernel, 0, sizeof(double),  (void *)&alpha);
  clStatus |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&A_clmem);
  clStatus |= clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&B_clmem);
  OCL_LOG(clStatus, "clSetKernelArg " );

  //LOG_OCL_COMPILER_ERROR(program, devices[pidx][didx]);
  double   ndrange_start;
  double   ndrange_stop;
  double 	ocl_time_host;

  size_t global_size[] = {64/4,4}; 
  size_t local_size[] = {4,4}; 
  cl_uint work_dim = 2;

  cl_event saxpy_event = NULL;
  ndrange_start=get_second();
  clStatus = clEnqueueNDRangeKernel(command_queue, kernel, work_dim, NULL,global_size, local_size, 0, NULL, &saxpy_event);
  OCL_LOG(clStatus, "clEnqueueNDRangeKernel " );
  clStatus = clWaitForEvents(1, &saxpy_event);
  OCL_LOG(clStatus, "clWaitForEvents ");
  ndrange_stop=get_second();
  ocl_time_host = (double) (ndrange_stop - ndrange_start);
  printf("NDRange perf. counter time %f ms.\n", 1000.0f*ocl_time_host);



  // Read the memory buffer C_clmem on the device to the host allocated buffer C
  // This task is invoked only after the completion of the event saxpy_event
  clStatus = clEnqueueReadBuffer(command_queue, A_clmem, CL_TRUE, 0,
		VECTOR_SIZE * sizeof(double), A, 1, &saxpy_event, NULL);
  OCL_LOG(clStatus, "clEnqueueReadBuffer " );
  clStatus = clEnqueueReadBuffer(command_queue, B_clmem, CL_TRUE, 0,
		VECTOR_SIZE * sizeof(double), B, 1, &saxpy_event, NULL);
  OCL_LOG(clStatus, "clEnqueueReadBuffer " );

  // Clean up and wait for all the comands to complete.
  clStatus = clFinish(command_queue);

  // Display the result to the screen
  /*
  for(int i = 0; i < VECTOR_SIZE; i++){
	 printf("[%f,%f]\n", A[i],B[i]);
  }
  */

  // Finally release all OpenCL objects and release the host buffers.
  clStatus = clReleaseKernel(kernel);
  clStatus = clReleaseMemObject(A_clmem);
  clStatus = clReleaseMemObject(B_clmem);
  clStatus = clReleaseCommandQueue(command_queue);
  clStatus = clReleaseContext(context);
  free(A);
  free(B);
  return 0;
}
