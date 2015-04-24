CC=gcc
CFLAGS=-Wall
LDFLAGS=
OBJECTS := $(addsuffix .cpp.o, $(basename $(wildcard *.cpp)))
EXECUTABLE=opencl

#OPENCL_DIR=/opt/cuda
OPENCL_DIR=/opt/intel/opencl-sdk
OPENCL_INC=$(OPENCL_DIR)/include
OPENCL_LIB=$(OPENCL_DIR)/lib64

LIBRARIES=-lOpenCL

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) -L$(OPENCL_LIB) $(LIBRARIES) $(OBJECTS) -o $@ 

%.cpp.o : %.cpp
	$(CC) -c $(CFLAGS) -I$(OPENCL_INC) $< -o $@

clean:
	rm -f *.o *~ $(EXECUTABLE)
