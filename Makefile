CC=nvcc
#CC=icc
CFLAGS=
LDFLAGS=
OBJECTS := $(addsuffix .cpp.o, $(basename $(wildcard *.cpp)))
EXECUTABLE=opencl

LIBRARIES=-lOpenCL

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(LIBRARIES) $(OBJECTS) -o $@ 

%.cpp.o : %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~ $(EXECUTABLE)
