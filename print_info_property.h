#ifndef _GET_PROPERTY
#define _GET_PROPERTY

#include <stdio.h>
#include <stdlib.h>
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include "ocl_macros.h"

void PrintDeviceInfo(cl_device_id device);
void PrintPlatformInfo(cl_platform_id platform);

#endif
