# Opencl Notes

Simple OpenCL vector add example

## Install OpenCL

   `install opencl`

### INTEL

   `intel`

### GPU   

   `cuda`

## Compile   

compilation notes

###  on Phi
```bash
source /opt/intel/composerxe/bin/compilervars.sh intel64
Makefile:
CC = icc
```

### on Cuda

```bash
export PATH=/usr/local/cuda/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH

Makefile:
CC = nvcc
```
