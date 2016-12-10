CXXFLAGS = -O3 -Wall
LIBS = -lz
DEPS = ../kxsort.h Makefile

all: exampleP64

%: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)