#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <CL/cl.h>


char *read_source(const char *fileName);
void check_errors(cl_int status);
