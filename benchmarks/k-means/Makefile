EXECS= k_means
CC=gcc
MPICC=mpicc

FLAGS=-O3 -Wall -g -lm 

all: ${EXECS}

k_means: k_means.c
	${MPICC} ${FLAGS} -o k_means $^ 

clean:
	rm k_means
	rm k_means.dSYM
