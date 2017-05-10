#!/bin/bash

#RUNNER="ibrun -o 0 -n"
RUNNER="mpirun -np"

array=(4 8 16 32 64)

echo "DEBUG: ssort"
for i in "${array[@]}"
do
	N=$((10**6*$i))
	$RUNNER $i ssort/ssort $N ssort/
	rm ssort/output*.txt

done

echo "DEBUG: jacobi-2D"
for i in "${array[@]}"
do
	N=$((10**3*$i))
	$RUNNER $i jacobi-2D/jacobi-mpi2D-nonBlocking $N 100

done

echo "DEBUG: k-means 1m"
for i in "${array[@]}"
do
	N=$((10**6*$i))
	python k-means/create_data.py $N k-means/${i}m.data &&
	$RUNNER $i k-means/k_means 3 $N 9 k-means/${i}m.data 10
	rm k-means/${i}m.data
done

echo "DEBUG: k-means 1e5"
for i in "${array[@]}"
do
	N=$((10**5*$i))
	python k-means/create_data.py $N k-means/${i}m.data &&
	$RUNNER $i k-means/k_means 3 $N 9 k-means/${i}m.data 100
	rm k-means/${i}m.data
done