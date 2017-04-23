CC=mpicc
FLAGS=-O3 -lm
EXECS= jacobi-mpi2D-nonBlocking

all: ${EXECS}

jacobi-mpi2D-nonBlocking: jacobi-mpi2D-nonBlocking.c
	${CC} ${FLAGS} $^ -o $@

clean:
	rm -rf jacobi-mpi2D-nonBlocking jacobi-mpi2D-nonBlocking.dSYM
