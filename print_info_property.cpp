#include "print_info_property.h"

void PrintPlatformInfo(cl_platform_id platform)
{
    char queryBuffer[1024];
    cl_int clError;

    clError = clGetPlatformInfo (platform, CL_PLATFORM_NAME, 1024, &queryBuffer, NULL);
    if(clError == CL_SUCCESS)
    {
        printf("CL_PLATFORM_NAME   : %s\n", queryBuffer);
    }
    clError = clGetPlatformInfo (platform, CL_PLATFORM_VENDOR, 1024, &queryBuffer, NULL);
    if(clError == CL_SUCCESS)
    {
        printf("CL_PLATFORM_VENDOR : %s\n", queryBuffer);
    }
    clError = clGetPlatformInfo (platform, CL_PLATFORM_VERSION, 1024, &queryBuffer, NULL);
    if (clError == CL_SUCCESS)
    {
        printf("CL_PLATFORM_VERSION: %s\n", queryBuffer);
    }
    clError = clGetPlatformInfo (platform, CL_PLATFORM_PROFILE, 1024, &queryBuffer, NULL);
    if (clError == CL_SUCCESS)
    {
        printf("CL_PLATFORM_PROFILE: %s\n", queryBuffer);
    }
    clError = clGetPlatformInfo (platform, CL_PLATFORM_EXTENSIONS, 1024, &queryBuffer, NULL);
    if (clError == CL_SUCCESS)
    {
        printf("CL_PLATFORM_EXTENSIONS: %s\n", queryBuffer);
    }
    return;
}

void PrintDeviceInfo(cl_device_id device)
{
    char queryBuffer[1024];
    int queryInt;
	cl_device_type device_type;
    cl_int clError;

    clError = clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(device_type), &device_type, NULL);

	if (device_type == CL_DEVICE_TYPE_GPU){
		 printf("CL_DEVICE_TYPE: GPU\n");
	}

	else if (device_type == CL_DEVICE_TYPE_CPU){
		 printf("CL_DEVICE_TYPE: CPU\n");
	}
	else if (device_type == CL_DEVICE_TYPE_ACCELERATOR){
		 printf("CL_DEVICE_TYPE: ACCELERATOR\n");
	}

    queryBuffer[0] = '\0';
    clError = clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(queryBuffer), &queryBuffer, NULL);
    printf("CL_DEVICE_NAME: %s\n", queryBuffer);
    queryBuffer[0] = '\0';
    clError = clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(queryBuffer), &queryBuffer, NULL);
    printf("CL_DEVICE_VENDOR: %s\n", queryBuffer);
    queryBuffer[0] = '\0';
    clError = clGetDeviceInfo(device, CL_DRIVER_VERSION, sizeof(queryBuffer), &queryBuffer, NULL);
    printf("CL_DRIVER_VERSION: %s\n", queryBuffer);
    queryBuffer[0] = '\0';
    clError = clGetDeviceInfo(device, CL_DEVICE_VERSION, sizeof(queryBuffer), &queryBuffer, NULL);
    printf("CL_DEVICE_VERSION: %s\n", queryBuffer);
    queryBuffer[0] = '\0';
    clError = clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(int), &queryInt, NULL);
    printf("CL_DEVICE_MAX_COMPUTE_UNITS: %d\n", queryInt);
}
