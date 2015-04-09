//If you want to build the file directly at the command prompt then use the following commands.
//AMD commands
//cl /c saxpy.cpp /I"%AMDAPPSDKROOT%\include"
//link  /OUT:"saxpy.exe" "%AMDAPPSDKROOT%\lib\x86_64\OpenCL.lib" saxpy.obj

//nVIDIA commands
//cl /c saxpy.cpp /I"%NVSDKCOMPUTE_ROOT%\OpenCL\common\inc"
//link  /OUT:"saxpy.exe" "%NVSDKCOMPUTE_ROOT%\OpenCL\common\lib\x64\OpenCL.lib" saxpy.obj

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif
#include "ocl_macros.h"
#include "print_info_property.h"

//#define DEVICE_TYPE CL_DEVICE_TYPE_GPU
#define DEVICE_TYPE CL_DEVICE_TYPE_ALL
//#define DEVICE_TYPE CL_DEVICE_TYPE_CPU
//#define DEVICE_TYPE CL_DEVICE_TYPE_ACCELERATOR
//#define DEVICE_TYPE CL_DEVICE_TYPE_DEFAULT

#define VECTOR_SIZE 1024
#define MAX_SOURCE_SIZE (0x100000)

int main(void) {

   FILE *fp;
   char *source_str;
   size_t source_size;

   fp = fopen("saxpy_kernel.cl","r");
   if (!fp) {
	  fprintf(stdout,"Failed to load kernel.\n");
	  exit(1);
   }
   source_str = (char*)malloc(MAX_SOURCE_SIZE);
   source_size = fread(source_str,1,MAX_SOURCE_SIZE,fp);
   fclose(fp);

   cl_int clStatus;
   cl_uint num_platforms = 0;

   cl_platform_id * platforms = NULL;

   cl_device_id **devices = NULL;
   cl_uint *num_devices = NULL;

   if( clGetPlatformIDs(0, NULL, &num_platforms) != CL_SUCCESS)
   {
	  printf("Error in call to clGetPlatformIDs....\n Exiting");
	  exit(-1);
   }
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

	  if( clGetPlatformIDs (num_platforms, platforms, NULL) != CL_SUCCESS)
	  {
		 free(platforms);
		 free(num_devices);
		 exit(-1);
	  }

	  for(cl_uint pidx=0;pidx<num_platforms; pidx++)
	  {
		 printf("==================Platform No %d======================\n",pidx);
		 PrintPlatformInfo(platforms[pidx]);
		 printf("======================================================\n\n");            

		 if ( clGetDeviceIDs (platforms[pidx], CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices[pidx]) != CL_SUCCESS)
		 {
			printf("ERROR Getting number of devices... Exiting\n ");
			exit(-1);
		 }

		 devices[pidx] = (cl_device_id *)malloc(num_devices[pidx]*sizeof(cl_device_id));

	  }


	  for(cl_uint pidx=0;pidx<num_platforms; pidx++)
	  { 
		 printf("\nPrinting OpenCL Device Info For Platform ID : %d\n", pidx);
		 if ( clGetDeviceIDs(platforms[pidx], CL_DEVICE_TYPE_ALL, num_devices[pidx], devices[pidx], NULL) != CL_SUCCESS)
		 {
			//free devices[pidx] free devices
			printf("ERROR Creating devices... Exiting\n ");
			exit(-1);
		 }

		 for (cl_uint didx = 0; didx < num_devices[pidx]; didx++)
		 {
			printf("==================Device No %d======================\n",didx);
			PrintDeviceInfo(devices[pidx][didx]);
			printf("====================================================\n\n");
		 }
	  }

   }
   // Create OpenCL context for devices in device_list
   cl_context context;
   cl_context_properties props[3] =
   {
	  CL_CONTEXT_PLATFORM,
	  (cl_context_properties)platforms[0],
	  0
   };
   // An OpenCL context can be associated to multiple devices, either CPU or GPU
   // based on the value of DEVICE_TYPE defined above.
   context = clCreateContext( NULL, num_devices[0], devices[0], NULL, NULL, &clStatus);
   LOG_OCL_ERROR(clStatus, "clCreateContext Failed..." );

   // Create a command queue for the first device in device_list
   cl_command_queue command_queue = clCreateCommandQueue(context, devices[0][0], 0, &clStatus);
   LOG_OCL_ERROR(clStatus, "clCreateCommandQueue Failed..." );

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
   LOG_OCL_ERROR(clStatus, "clEnqueueWriteBuffer Failed..." );
   clStatus = clEnqueueWriteBuffer(command_queue, B_clmem, CL_TRUE, 0,
		 VECTOR_SIZE * sizeof(float), B, 0, NULL, NULL);
   LOG_OCL_ERROR(clStatus, "clEnqueueWriteBuffer Failed..." );

   // Create a program from the kernel source
   cl_program program = clCreateProgramWithSource(context, 1,
		 (const char **)&source_str, NULL, &clStatus);
   LOG_OCL_ERROR(clStatus, "clCreateProgramWithSource Failed..." );

   // Build the program
   clStatus = clBuildProgram(program, 1, devices[0], NULL, NULL, NULL);
   if(clStatus != CL_SUCCESS)
	  LOG_OCL_COMPILER_ERROR(program, devices[0][0]);

   // Create the OpenCL kernel
   cl_kernel kernel = clCreateKernel(program, "saxpy_kernel", &clStatus);

   // Set the arguments of the kernel. Take a look at the kernel definition in saxpy_kernel
   // variable. First parameter is a constant and the other three are buffers.
   clStatus = clSetKernelArg(kernel, 0, sizeof(float),  (void *)&alpha);
   clStatus |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&A_clmem);
   clStatus |= clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&B_clmem);
   clStatus |= clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&C_clmem);
   LOG_OCL_ERROR(clStatus, "clSetKernelArg Failed..." );

   // Execute the OpenCL kernel on the list
   size_t global_size = VECTOR_SIZE; // Process one vector element in each work item
   size_t local_size = 64;           // Process in work groups of size 64.
   cl_event saxpy_event;
   clStatus = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
		 &global_size, &local_size, 0, NULL, &saxpy_event);
   LOG_OCL_ERROR(clStatus, "clEnqueueNDRangeKernel Failed..." );

   // Read the memory buffer C_clmem on the device to the host allocated buffer C
   // This task is invoked only after the completion of the event saxpy_event
   clStatus = clEnqueueReadBuffer(command_queue, C_clmem, CL_TRUE, 0,
		 VECTOR_SIZE * sizeof(float), C, 1, &saxpy_event, NULL);
   LOG_OCL_ERROR(clStatus, "clEnqueueReadBuffer Failed..." );

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

