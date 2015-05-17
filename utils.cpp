#include "utils.h"
#include "ocl_macros.h"

#define MAX_NAME_LEN 80

void check_errors(cl_int status)
{

#define CASE_CL_CONSTANT(NAME) case NAME: printf("%s\n",#NAME); return;

   switch (status)
   {
	  CASE_CL_CONSTANT(CL_SUCCESS)
		 CASE_CL_CONSTANT(CL_BUILD_PROGRAM_FAILURE)
		 CASE_CL_CONSTANT(CL_COMPILER_NOT_AVAILABLE)
		 CASE_CL_CONSTANT(CL_DEVICE_NOT_AVAILABLE)
		 CASE_CL_CONSTANT(CL_DEVICE_NOT_FOUND)
		 CASE_CL_CONSTANT(CL_IMAGE_FORMAT_MISMATCH)
		 CASE_CL_CONSTANT(CL_IMAGE_FORMAT_NOT_SUPPORTED)
		 CASE_CL_CONSTANT(CL_INVALID_ARG_INDEX)
		 CASE_CL_CONSTANT(CL_INVALID_ARG_SIZE)
		 CASE_CL_CONSTANT(CL_INVALID_ARG_VALUE)
		 CASE_CL_CONSTANT(CL_INVALID_BINARY)
		 CASE_CL_CONSTANT(CL_INVALID_BUFFER_SIZE)
		 CASE_CL_CONSTANT(CL_INVALID_BUILD_OPTIONS)
		 CASE_CL_CONSTANT(CL_INVALID_COMMAND_QUEUE)
		 CASE_CL_CONSTANT(CL_INVALID_CONTEXT)
		 CASE_CL_CONSTANT(CL_INVALID_DEVICE)
		 CASE_CL_CONSTANT(CL_INVALID_DEVICE_TYPE)
		 CASE_CL_CONSTANT(CL_INVALID_EVENT)
		 CASE_CL_CONSTANT(CL_INVALID_EVENT_WAIT_LIST)
		 CASE_CL_CONSTANT(CL_INVALID_GL_OBJECT)
		 CASE_CL_CONSTANT(CL_INVALID_GLOBAL_OFFSET)
		 CASE_CL_CONSTANT(CL_INVALID_HOST_PTR)
		 CASE_CL_CONSTANT(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR)
		 CASE_CL_CONSTANT(CL_INVALID_IMAGE_SIZE)
		 CASE_CL_CONSTANT(CL_INVALID_KERNEL_NAME)
		 CASE_CL_CONSTANT(CL_INVALID_KERNEL)
		 CASE_CL_CONSTANT(CL_INVALID_KERNEL_ARGS)
		 CASE_CL_CONSTANT(CL_INVALID_KERNEL_DEFINITION)
		 CASE_CL_CONSTANT(CL_INVALID_MEM_OBJECT)
		 CASE_CL_CONSTANT(CL_INVALID_OPERATION)
		 CASE_CL_CONSTANT(CL_INVALID_PLATFORM)
		 CASE_CL_CONSTANT(CL_INVALID_PROGRAM)
		 CASE_CL_CONSTANT(CL_INVALID_PROGRAM_EXECUTABLE)
		 CASE_CL_CONSTANT(CL_INVALID_QUEUE_PROPERTIES)
		 CASE_CL_CONSTANT(CL_INVALID_SAMPLER)
		 CASE_CL_CONSTANT(CL_INVALID_VALUE)
		 CASE_CL_CONSTANT(CL_INVALID_WORK_DIMENSION)
		 CASE_CL_CONSTANT(CL_INVALID_WORK_GROUP_SIZE)
		 CASE_CL_CONSTANT(CL_INVALID_WORK_ITEM_SIZE)
		 CASE_CL_CONSTANT(CL_MAP_FAILURE)
		 CASE_CL_CONSTANT(CL_MEM_OBJECT_ALLOCATION_FAILURE)
		 CASE_CL_CONSTANT(CL_MEM_COPY_OVERLAP)
		 CASE_CL_CONSTANT(CL_OUT_OF_HOST_MEMORY)
		 CASE_CL_CONSTANT(CL_OUT_OF_RESOURCES)
		 CASE_CL_CONSTANT(CL_PROFILING_INFO_NOT_AVAILABLE)
   }
   return;
}

char* read_source(const char *filename) 
{
   FILE *file = fopen(filename, "rb");
   if (!file) {
	  printf("ERROR) Failed to open file '%s'\n", filename);
	  return NULL;
   }

   if (fseek(file, 0, SEEK_END)) {
	  printf("ERROR) Failed to seek file '%s'\n", filename);
	  fclose(file);
	  return NULL;
   }

   long size = ftell(file);
   if (size == 0) {
	  printf("ERROR) Failed to check position on file '%s'\n", filename);
	  fclose(file);
	  return NULL;
   }

   rewind(file);

   char *src = (char *) malloc(sizeof(char) * size + 1);
   if (!src) {
	  printf("ERROR) Failed to allocate memory for file '%s'\n", filename);
	  fclose(file);
	  return NULL;
   }

   printf("Reading file '%s' (size %ld bytes)\n", filename, size);
   size_t res = fread(src, 1, sizeof(char) * size, file);
   if (res != sizeof(char) * size) {
	  printf("ERROR) Failed to read file '%s'\n", filename);
	  fclose(file);
	  free(src);
	  return NULL;
   }

   src[size] = '\0'; /* NULL terminated */
   fclose(file);

   return src;
}

void create_context(int pidx, int didx, cl_context *contex, cl_command_queue *command_queue, int enable_profiling)
{
   cl_int status;
   size_t platform_name_length = 0;
   size_t device_name_length = 0;
   // get number of platforms
   cl_uint num_platforms;
   OCL_CHECK(clGetPlatformIDs(0, NULL, &num_platforms));

   // allocate memory, get list of platform handles
   cl_platform_id *platforms = (cl_platform_id *) malloc(num_platforms*sizeof(cl_platform_id));
   SYS_LOG(!platforms, "allocating platform array");
   OCL_CHECK(clGetPlatformIDs (num_platforms, platforms, NULL));
   OCL_CHECK(clGetPlatformInfo (platforms[pidx], CL_PLATFORM_NAME,	0, 0, &platform_name_length));
   char* platform_name = (char*) malloc(platform_name_length*sizeof(char));
   OCL_CHECK(clGetPlatformInfo (platforms[pidx], CL_PLATFORM_NAME,	platform_name_length, platform_name, NULL));
   printf("platform %d: name '%s'\n", pidx, platform_name);
   free(platform_name);

   // get number of devices in platform
   cl_uint num_devices;
   OCL_CHECK(clGetDeviceIDs (platforms[pidx], CL_DEVICE_TYPE_ALL,0, NULL, &num_devices));

   // allocate memory, get list of device handles in platform
   cl_device_id *devices = (cl_device_id *) malloc(num_devices*sizeof(cl_device_id));
   SYS_LOG(!devices, "allocating device array");

   OCL_CHECK(clGetDeviceIDs (platforms[pidx], CL_DEVICE_TYPE_ALL,num_devices, devices, NULL));
   OCL_CHECK(clGetDeviceInfo (devices[didx], CL_DEVICE_NAME,0,0, &device_name_length));
   char* device_name = (char*) malloc(device_name_length*sizeof(char));

   OCL_CHECK(clGetDeviceInfo (devices[didx], CL_DEVICE_NAME,device_name_length, device_name, NULL));
   cl_uint num_compute_units;
   OCL_CHECK(clGetDeviceInfo (devices[didx], CL_DEVICE_MAX_COMPUTE_UNITS,sizeof(cl_uint), &num_compute_units, NULL));
   printf("device %d: name '%s'\ncompute units: %d\n", didx, device_name,num_compute_units);
   free(device_name);
   
   //Create Context
   cl_context_properties cprops[3] = {
	  CL_CONTEXT_PLATFORM, (cl_context_properties) platforms[pidx], 0 };

   *contex = clCreateContext(cprops, 1, &devices[didx], NULL, NULL, &status);
   OCL_LOG(status, "clCreateContext");
   cl_command_queue_properties qprops = 0;
   if (enable_profiling)
	  qprops |= CL_QUEUE_PROFILING_ENABLE;
   //Create command queue for device
   if (command_queue)
   {
	  *command_queue = clCreateCommandQueue(*contex, devices[didx], qprops, &status);
	  OCL_LOG(status, "clCreateCommandQueue");
   }
   free(devices);
   free(platforms);
   return;
}

cl_kernel build_kernel(cl_context context,char const *kernel_source, char const *kernel_name, char const *options)
{
  // create an OpenCL program (may have multiple kernels)
  size_t sizes[] = { strlen(kernel_source) };

  if (options && strlen(options) == 0)
  {
    // reportedly, some implementations dislike empty strings.
    options = NULL;
  }

  cl_int status;
  cl_program program = clCreateProgramWithSource(context, 1, &kernel_source, sizes, &status);
  OCL_LOG(status, "clCreateProgramWithSource");

  // build it
  status = clBuildProgram(program, 0, NULL, options, NULL, NULL);
  OCL_LOG(status, "clBuildProgram");
  cl_device_id dev;
  OCL_CHECK(clGetProgramInfo (program, CL_PROGRAM_DEVICES,sizeof(dev), &dev, NULL));
  if(status != CL_SUCCESS)
	 LOG_OCL_COMPILER_ERROR(program, dev);



  // fish the kernel out of the program
  cl_kernel kernel = clCreateKernel(program, kernel_name, &status);
  OCL_LOG(status, "clCreateKernel");

  OCL_CHECK(clReleaseProgram (program));

  return kernel;
}

void print_platforms_devices(void)
{
   // get number of platforms
   cl_uint num_platforms;
   OCL_CHECK( clGetPlatformIDs(0, NULL, &num_platforms));

   // allocate memory, get list of platforms
   cl_platform_id *platforms = (cl_platform_id *) malloc(num_platforms*sizeof(cl_platform_id));
   SYS_LOG(!platforms, "malloc platrforms");

   OCL_CHECK(clGetPlatformIDs(num_platforms, platforms, NULL));

   // iterate over platforms
   for (cl_uint pidx = 0; pidx < num_platforms; pidx++)
   {
	  // get platform vendor name
	  char buf[MAX_NAME_LEN];
	  OCL_CHECK(clGetPlatformInfo (platforms[pidx], CL_PLATFORM_VENDOR,sizeof(buf), buf, NULL));
	  printf("platform %d: vendor '%s'\n", pidx, buf);

	  // get number of devices in platform
	  cl_uint num_devices;
	  OCL_CHECK(clGetDeviceIDs(platforms[pidx], CL_DEVICE_TYPE_ALL,0, NULL, &num_devices));

	  cl_device_id *devices = (cl_device_id *) malloc(num_devices*sizeof(cl_device_id));
	  SYS_LOG(!devices, "malloc devices");

	  // get list of devices in platform
	  OCL_CHECK(clGetDeviceIDs (platforms[pidx], CL_DEVICE_TYPE_ALL,num_devices, devices, NULL));

	  // iterate over devices
	  for (cl_uint didx = 0; didx < num_devices; didx++)
	  {
		 char buf[MAX_NAME_LEN];
		 OCL_CHECK(clGetDeviceInfo (devices[didx], CL_DEVICE_NAME,sizeof(buf), buf, NULL));
		 printf("  device %d: '%s'\n", didx, buf);
	  }

	  free(devices);
   }

   free(platforms);
}

