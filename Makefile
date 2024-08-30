
CC = g++

CDEFS=
CFLAGS= -g -Wall -fopenmp $(INCLUDE_DIRS) $(CDEFS)

PRODUCT= geneticalgo omp_geneticalgo

HFILES=
CPPFILES= geneticalgo.cpp omp_geneticalgo.cpp

SRCS= ${HFILES} ${CPPFILES}
OBJS= ${CPPFILES:.c=.o}

all:	${PRODUCT}

clean:
	-rm -f *.o *.NEW *~
	-rm -f ${PRODUCT} ${DERIVED} ${GARBAGE}

geneticalgo: geneticalgo.cpp
	$(CC) $(CFLAGS) -o $@ geneticalgo.cpp $(LIB_DIRS)

omp_geneticalgo: omp_geneticalgo.cpp
	$(CC) $(CFLAGS) -o $@ omp_geneticalgo.cpp $(LIB_DIRS)
