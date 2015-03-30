CC=gcc
CFLAGS=-Wall
LDFLAGS=
SOURCES=saxpy.cpp
OBJECTS := $(addsuffix .cpp.o, $(basename $(wildcard *.cpp)))
#OBJECTS=$(SOURCES:.cpp.o)
EXECUTABLE=saxpy

CUDA=/opt/cuda
OPENCL_INC=$(CUDA)/include
OPENCL_LIB=$(CUDA)/lib64

LIBRARIES=-lOpenCL

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) -L$(OPENCL_LIB) $(OBJECTS) -o $@ $(LIBRARIES)

$(OBJECTS): $(SOURCES)
	$(CC) -c $(CFLAGS) -I$(OPENCL_INC) $< -o $@

clean:
	rm -f *.o *~ $(EXECUTABLE)
