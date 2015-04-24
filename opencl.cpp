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

#define VECTOR_SIZE 16777216

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
  char *source_str = read_source("saxpy_kernel.cl");
  cl_kernel kernel = build_kernel(context, source_str, "saxpy_kernel", NULL);
  free(source_str);


   float alpha = 2.0;
   // Allocate space for vectors A, B and C
   float *A = (float*)malloc(sizeof(float)*VECTOR_SIZE);
   float *B = (float*)malloc(sizeof(float)*VECTOR_SIZE);
   float *C = (float*)malloc(sizeof(float)*VECTOR_SIZE);
   for(int i = 0; i < VECTOR_SIZE; i++)
   {
	  A[i] = (float)i;
	  B[i] = (float)(VECTOR_SIZE - i);
	  C[i] = 0;
   }

   // Create memory buffers on the device for each vector
   cl_mem A_clmem = clCreateBuffer(context, CL_MEM_READ_ONLY,
		 VECTOR_SIZE * sizeof(float), NULL, &clStatus);
   cl_mem B_clmem = clCreateBuffer(context, CL_MEM_READ_ONLY,
		 VECTOR_SIZE * sizeof(float), NULL, &clStatus);
   cl_mem C_clmem = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
		 VECTOR_SIZE * sizeof(float), NULL, &clStatus);

   // Copy the Buffer A and B to the device. We do a blocking write to the device buffer.
   clStatus = clEnqueueWriteBuffer(command_queue, A_clmem, CL_TRUE, 0,
		 VECTOR_SIZE * sizeof(float), A, 0, NULL, NULL);
   OCL_LOG(clStatus, "clEnqueueWriteBuffer " );
   clStatus = clEnqueueWriteBuffer(command_queue, B_clmem, CL_TRUE, 0,
		 VECTOR_SIZE * sizeof(float), B, 0, NULL, NULL);
   OCL_LOG(clStatus, "clEnqueueWriteBuffer " );

   // Create a program from the kernel source
   cl_program program = clCreateProgramWithSource(context, 1,
		 (const char **)&source_str, NULL, &clStatus);
   OCL_LOG(clStatus, "clCreateProgramWithSource " );

   // Build the program
   //clStatus = clBuildProgram(program, 1, devices[pidx], NULL, NULL, NULL);
   //LOG_OCL_COMPILER_ERROR(program, devices[pidx][didx]);


   // Create the OpenCL kernel
   kernel = clCreateKernel(program, "saxpy_kernel", &clStatus);

   // Set kernel arguments
   clStatus = clSetKernelArg(kernel, 0, sizeof(float),  (void *)&alpha);
   clStatus |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&A_clmem);
   clStatus |= clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&B_clmem);
   clStatus |= clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&C_clmem);
   OCL_LOG(clStatus, "clSetKernelArg " );

   //LOG_OCL_COMPILER_ERROR(program, devices[pidx][didx]);
   double   ndrange_start;
   double   ndrange_stop;
   float 	ocl_time_host;

   size_t global_size = VECTOR_SIZE; 
   size_t local_size = 64;
   //OCL_CHECK( clGetDeviceInfo(devices[pidx][didx], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &local_size, NULL));
   cl_uint work_dim = 1;

   cl_event saxpy_event = NULL;
   ndrange_start=get_second();
   //clStatus = clEnqueueNDRangeKernel(command_queue, kernel, work_dim, NULL,&global_size, &local_size, 0, NULL, &saxpy_event);
   //OpenCL can choose work distribution automatically
   clStatus = clEnqueueNDRangeKernel(command_queue, kernel, work_dim, NULL,&global_size, NULL, 0, NULL, &saxpy_event);
   OCL_LOG(clStatus, "clEnqueueNDRangeKernel " );
   clStatus = clWaitForEvents(1, &saxpy_event);
   OCL_LOG(clStatus, "clWaitForEvents ");
   ndrange_stop=get_second();
   ocl_time_host = (float) (ndrange_stop - ndrange_start);
   printf("NDRange perf. counter time %f ms.\n", 1000.0f*ocl_time_host);

   

   // Read the memory buffer C_clmem on the device to the host allocated buffer C
   // This task is invoked only after the completion of the event saxpy_event
   clStatus = clEnqueueReadBuffer(command_queue, C_clmem, CL_TRUE, 0,
		 VECTOR_SIZE * sizeof(float), C, 1, &saxpy_event, NULL);
   OCL_LOG(clStatus, "clEnqueueReadBuffer " );

   // Clean up and wait for all the comands to complete.
   clStatus = clFinish(command_queue);

   // Display the result to the screen
   for(int i = VECTOR_SIZE -2; i < VECTOR_SIZE; i++)
	  printf("%f * %f + %f = %f\n", alpha, A[i], B[i], C[i]);

   // Finally release all OpenCL objects and release the host buffers.
   clStatus = clReleaseKernel(kernel);
   clStatus = clReleaseProgram(program);
   clStatus = clReleaseMemObject(A_clmem);
   clStatus = clReleaseMemObject(B_clmem);
   clStatus = clReleaseMemObject(C_clmem);
   clStatus = clReleaseCommandQueue(command_queue);
   clStatus = clReleaseContext(context);
   free(A);
   free(B);
   free(C);
   return 0;
}
