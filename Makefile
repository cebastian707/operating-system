
CFLAG=-std=c++20
boundaryTagApp.x: BoundaryTag.o driver2.o
	g++ $(CFLAGS)  BoundaryTag.o driver2.o -o boundaryTagApp.x

BoundaryTag.o: BoundaryTag.cpp BoundaryTag.hpp
	g++  $(CFLAGS) -c BoundaryTag.cpp -o BoundaryTag.o

driver2.o: driver2.cpp BoundaryTag.hpp
	g++  $(CFLAGS) -c driver2.cpp -o driver2.o

clean:
	rm -f BoundaryTag.o driver2.o boundaryTagApp.x core *~
