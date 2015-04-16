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

   char *source_str = read_source("saxpy_kernel.cl");

   cl_int clStatus;
   cl_uint num_platforms = 0;

   cl_platform_id * platforms = NULL;

   cl_device_id **devices = NULL;
   cl_uint *num_devices = NULL;

   OCL_CHECK( clGetPlatformIDs(0, NULL, &num_platforms));
   if (num_platforms == 0)
   {
	  printf("No OpenCL Platforms Found ....\n Exiting");
   }
   else
   {

	  printf ("Found %d Platforms\n", num_platforms);
	  platforms = (cl_platform_id *)malloc(num_platforms*sizeof(cl_platform_id));
	  num_devices = (cl_uint *)malloc(num_platforms*sizeof(cl_uint));
	  devices = (cl_device_id **)malloc(num_platforms*sizeof(cl_device_id*));

	  OCL_CHECK( clGetPlatformIDs (num_platforms, platforms, NULL) );

	  for(cl_uint pidx=0;pidx<num_platforms; pidx++)
	  {
		 printf("==================Platform No %d======================\n",pidx);
		 PrintPlatformInfo(platforms[pidx]);
		 printf("======================================================\n\n");            

		 OCL_CHECK(clGetDeviceIDs (platforms[pidx], CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices[pidx]));

		 devices[pidx] = (cl_device_id *)malloc(num_devices[pidx]*sizeof(cl_device_id));

	  }


	  for(cl_uint pidx=0;pidx<num_platforms; pidx++)
	  { 
		 printf("\nPrinting OpenCL Device Info For Platform ID : %d\n", pidx);
		 OCL_CHECK(clGetDeviceIDs(platforms[pidx], CL_DEVICE_TYPE_ALL, num_devices[pidx], devices[pidx], NULL));

		 for (cl_uint didx = 0; didx < num_devices[pidx]; didx++)
		 {
			printf("==================Device No %d======================\n",didx);
			PrintDeviceInfo(devices[pidx][didx]);
			printf("====================================================\n\n");
		 }
	  }

   }

   int pidx,didx;

   /* If no parameter passed to program print error
	  message and exit */
   if (argc < 3)
   {
	  printf("Usage: opencl [platform] [device]\n");
	  exit(-1); 
   }

   pidx = atoi(argv[1]); 
   didx = atoi(argv[2]);

   cl_context context;
   cl_context_properties props[3] =
   {
	  CL_CONTEXT_PLATFORM,
	  (cl_context_properties)platforms[pidx],
	  0
   };
   // An OpenCL context can be associated to multiple devices, either CPU or GPU
   // based on the value of DEVICE_TYPE defined above.
   context = clCreateContext( NULL, num_devices[pidx], devices[pidx], NULL, NULL, &clStatus);
   OCL_LOG(clStatus, "clCreateContext " );

   //cl_command_queue command_queue = clCreateCommandQueue(context, devices[pidx][didx], 0, &clStatus);
   cl_command_queue command_queue = clCreateCommandQueue(context, devices[pidx][didx], 0, &clStatus);
   OCL_LOG(clStatus, "clCreateCommandQueue " );

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
   clStatus = clBuildProgram(program, 1, devices[pidx], NULL, NULL, NULL);
   if(clStatus != CL_SUCCESS)
	  LOG_OCL_COMPILER_ERROR(program, devices[pidx][didx]);

   // Create the OpenCL kernel
   cl_kernel kernel = clCreateKernel(program, "saxpy_kernel", &clStatus);

   // Set kernel arguments
   clStatus = clSetKernelArg(kernel, 0, sizeof(float),  (void *)&alpha);
   clStatus |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&A_clmem);
   clStatus |= clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&B_clmem);
   clStatus |= clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&C_clmem);
   OCL_LOG(clStatus, "clSetKernelArg " );

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
   free(platforms);
   free(devices);
   return 0;
}
