#pragma OPENCL EXTENSION cl_khr_fp64 : enable
__kernel
void my_kernel(const double alpha,
                        __global double *A,
                        __global double *B
						)
{
//Get the index of the work-item
int gx = get_global_id(0);
int gy = get_global_id(1);
int Gx = get_global_size(0);
int Gy = get_global_size(1);

//   A[gx] = Gx;
 //  B[gx] = gx;
   A[gy] = Gy;
   B[gy] = gy;
}
