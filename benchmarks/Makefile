CC=mpicc
FLAGS=-O3 -lm

all: 
	$(MAKE) -C ssort
	$(MAKE) -C jacobi-2D
	$(MAKE) -C k-means

clean:
	$(MAKE) clean -C ssort
	$(MAKE) clean -C jacobi-2D	
	$(MAKE)  clean -C k-means