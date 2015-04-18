CC=gcc
CFLAGS=-Wall
LDFLAGS=
OBJECTS := $(addsuffix .cpp.o, $(basename $(wildcard *.cpp)))
EXECUTABLE=opencl

CUDA=/opt/cuda
OPENCL_INC=$(CUDA)/include
OPENCL_LIB=$(CUDA)/lib64

LIBRARIES=-lOpenCL

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) -L$(OPENCL_LIB) $(OBJECTS) -o $@ $(LIBRARIES)

%.cpp.o : %.cpp
	$(CC) -c $(CFLAGS) -I$(OPENCL_INC) $< -o $@

clean:
	rm -f *.o *~ $(EXECUTABLE)
