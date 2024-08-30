CC = g++
CFLAGS = -std=c++11 -Wall
OMPFLAGS = -fopenmp

all: sequential openmp

sequential: sequential.cpp
	$(CC) $(CFLAGS) -o sequential sequential.cpp

openmp: openmp.cpp
	$(CC) $(CFLAGS) $(OMPFLAGS) -o openmp openmp.cpp

clean:
	rm -f sequential openmp
