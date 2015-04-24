#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <CL/cl.h>
#include <string.h>


char *read_source(const char *fileName);
void check_errors(cl_int status);

void print_platforms_devices(void);
void create_context(int pidx, int didx, cl_context *contex, cl_command_queue *command_queue, int enable_profiling);
cl_kernel build_kernel(cl_context context,char const *kernel, char const *kernel_name, char const *options);
