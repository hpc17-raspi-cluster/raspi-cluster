CC=mpicc
FLAGS=-O3 -lm
EXECS= ssort

all: ${EXECS}

ssort: ssort.c
	${CC} ${FLAGS} $^ -o $@

clean:
	rm -rf ssort ssort.dSYM
	rm output*
