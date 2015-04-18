#include "print_info_property.h"

void PrintPlatformInfo(cl_platform_id platform)
{
    char query_buffer[1024];

    query_buffer[0] = '\0';
    OCL_CHECK( clGetPlatformInfo (platform, CL_PLATFORM_NAME, 1024, &query_buffer, NULL));
    printf("CL_PLATFORM_NAME   : %s\n", query_buffer);
    query_buffer[0] = '\0';
    OCL_CHECK( clGetPlatformInfo (platform, CL_PLATFORM_VENDOR, 1024, &query_buffer, NULL));
    printf("CL_PLATFORM_VENDOR : %s\n", query_buffer);
    query_buffer[0] = '\0';
    OCL_CHECK( clGetPlatformInfo (platform, CL_PLATFORM_VERSION, 1024, &query_buffer, NULL));
    printf("CL_PLATFORM_VERSION: %s\n", query_buffer);
    query_buffer[0] = '\0';
    OCL_CHECK( clGetPlatformInfo (platform, CL_PLATFORM_PROFILE, 1024, &query_buffer, NULL));
    printf("CL_PLATFORM_PROFILE: %s\n", query_buffer);
    query_buffer[0] = '\0';
    OCL_CHECK( clGetPlatformInfo (platform, CL_PLATFORM_EXTENSIONS, 1024, &query_buffer, NULL));
    printf("CL_PLATFORM_EXTENSIONS: %s\n", query_buffer);

    return;
}

void PrintDeviceInfo(cl_device_id device)
{
    char query_buffer[1024];
    cl_uint query_uint;
	cl_ulong query_ulong;
	cl_device_type device_type;

    OCL_CHECK( clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(device_type), &device_type, NULL));

	if (device_type == CL_DEVICE_TYPE_GPU){
		 printf("CL_DEVICE_TYPE: GPU\n");
	}

	else if (device_type == CL_DEVICE_TYPE_CPU){
		 printf("CL_DEVICE_TYPE: CPU\n");
	}
	else if (device_type == CL_DEVICE_TYPE_ACCELERATOR){
		 printf("CL_DEVICE_TYPE: ACCELERATOR\n");
	}

    query_buffer[0] = '\0';
    OCL_CHECK(clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(query_buffer), &query_buffer, NULL));
    printf("CL_DEVICE_NAME: %s\n", query_buffer);
	query_buffer[0] = '\0';
    OCL_CHECK( clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(query_buffer), &query_buffer, NULL));
    printf("CL_DEVICE_VENDOR: %s\n", query_buffer);
    query_buffer[0] = '\0';
    OCL_CHECK( clGetDeviceInfo(device, CL_DRIVER_VERSION, sizeof(query_buffer), &query_buffer, NULL));
    printf("CL_DRIVER_VERSION: %s\n", query_buffer);
    query_buffer[0] = '\0';
    OCL_CHECK( clGetDeviceInfo(device, CL_DEVICE_VERSION, sizeof(query_buffer), &query_buffer, NULL));
    printf("CL_DEVICE_VERSION: %s\n", query_buffer);
    query_buffer[0] = '\0';
    OCL_CHECK( clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(query_uint), &query_uint, NULL));
    printf("CL_DEVICE_MAX_COMPUTE_UNITS: %u\n", query_uint);
    OCL_CHECK( clGetDeviceInfo(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(query_uint), &query_uint, NULL));
    printf("CL_DEVICE_MAX_CLOCK_FREQUENCY: %u MHz (%.2f GHz)\n", query_uint, (float) query_uint/1000);
    OCL_CHECK( clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(query_ulong), &query_ulong, NULL));
    printf("CL_DEVICE_GLOBAL_MEM_SIZE: %lu MBytes (%lu bytes)\n", query_ulong/1048576,query_ulong);
    OCL_CHECK( clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(query_ulong), &query_ulong, NULL));
    printf("CL_DEVICE_GLOBAL_MEM_CACHE_SIZE (L2 cache): %lu KBytes (%lu bytes)\n", query_ulong/1024 ,query_ulong);
    OCL_CHECK( clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(query_ulong), &query_ulong, NULL));
    printf("CL_DEVICE_MAX_WORK_GROUP_SIZE: %llu\n", (unsigned long long)query_ulong);
    OCL_CHECK( clGetDeviceInfo(device, CL_DEVICE_ADDRESS_BITS, sizeof(query_uint), &query_uint, NULL));
    printf("CL_DEVICE_ADDRESS_BITS: %u\n", query_uint);

	size_t workitem_dims;
    OCL_CHECK( clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(workitem_dims), &workitem_dims, NULL));
    printf("CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: %u\n", (unsigned int)workitem_dims);
	size_t workitem_size[3];
    OCL_CHECK( clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(workitem_size), &workitem_size, NULL));
    printf("CL_DEVICE_MAX_WORK_ITEM_SIZES: %lu / %lu / %lu \n", workitem_size[0],workitem_size[1],workitem_size[2]);

	return;
}
