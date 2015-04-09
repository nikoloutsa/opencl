CC=gcc
CFLAGS=-Wall
LDFLAGS=
SOURCES=$(wildcard *.cpp)
#SOURCES=saxpy.cpp get_platform_property.cpp
OBJECTS=$(SOURCES:.cpp=.o)
#OBJECTS := $(addsuffix .cpp.o, $(basename $(wildcard *.cpp)))
#OBJECTS=$(SOURCES:.cpp.o)
EXECUTABLE=opencl

CUDA=/opt/cuda
OPENCL_INC=$(CUDA)/include
OPENCL_LIB=$(CUDA)/lib64

LIBRARIES=-lOpenCL

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) -L$(OPENCL_LIB) $(OBJECTS) -o $@ $(LIBRARIES)

%.cpp.o : %.cpp
	$(CC) -c $(CFLAGS) -I$(OPENCL_INC) $< -o $@

clean:
	rm -f *.o *~ $(EXECUTABLE)
