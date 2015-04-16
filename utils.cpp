#include "utils.h"

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

char* read_source(const char *filename) {
   FILE *file = fopen(filename, "rb");
   if (!file) {
	  printf("ERROR) Failed to open file '%s'\n", filename);
	  return NULL;
   }

   if (fseek(file, 0, SEEK_END)) {
	  printf("ERROR) Failed to seek file '%ls'\n", filename);
	  fclose(file);
	  return NULL;
   }

   long size = ftell(file);
   if (size == 0) {
	  printf("ERROR) Failed to check position on file '%ls'\n", filename);
	  fclose(file);
	  return NULL;
   }

   rewind(file);

   char *src = (char *) malloc(sizeof(char) * size + 1);
   if (!src) {
	  printf("ERROR) Failed to allocate memory for file '%ls'\n", filename);
	  fclose(file);
	  return NULL;
   }

   printf("Reading file '%ls' (size %ld bytes)\n", filename, size);
   size_t res = fread(src, 1, sizeof(char) * size, file);
   if (res != sizeof(char) * size) {
	  printf("ERROR) Failed to read file '%ls'\n", filename);
	  fclose(file);
	  free(src);
	  return NULL;
   }

   src[size] = '\0'; /* NULL terminated */
   fclose(file);

   return src;
}
