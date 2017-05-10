#!/bin/bash

#RUNNER="ibrun -o 0 -n"
RUNNER="mpirun -np"
#RUNNER="mpirun --hosts rsp1,rsp2,rsp3 -perhost 1 -np"
array=(4)

N=10000000
echo "DEBUG: ssort N:$N"
for i in "${array[@]}"
do
	echo "$RUNNER $i ssort/ssort $N ssort/"
	$RUNNER $i ssort/ssort $N ssort/
	rm ssort/output*.txt

done

N=10000
echo "DEBUG: jacobi-2D N:$N"
for i in "${array[@]}"
do
	echo "$RUNNER $i jacobi-2D/jacobi-mpi2D-nonBlocking $N 100"
	$RUNNER $i jacobi-2D/jacobi-mpi2D-nonBlocking $N 100

done

N=$((10**7))
echo "DEBUG: k-means N:$N"
python k-means/create_data.py $N k-means/${N}.data &&
for i in "${array[@]}"
do
	echo "$RUNNER $i k-means/k_means 3 $N 9 k-means/${N}.data 20"
	$RUNNER $i k-means/k_means 3 $N 9 k-means/${N}.data 20
	rm k-means/${N}.data
done

N=$((10**6))
echo "DEBUG: k-means N;$N"
python k-means/create_data.py $N k-means/${N}.data &&
for i in "${array[@]}"
do
	echo "$RUNNER $i k-means/k_means 3 $N 9 k-means/${N}.data 200"
	$RUNNER $i k-means/k_means 3 $N 9 k-means/${N}.data 200
	rm k-means/${N}.data
done