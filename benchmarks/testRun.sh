make all >/dev/null&&
echo 'LOG: Successfully compiles' &&
mpirun -np 10 ssort/ssort 100 ssort/ >/dev/null&&
echo 'LOG: ssort...good!'&&
mpirun -np 16 jacobi-2D/jacobi-mpi2D-nonBlocking 100 100 >/dev/null &&
echo 'LOG: jacobi-2D...good!' &&
mpirun -np 16 jacobi-2D/jacobi-mpi2D-nonBlocking 100 100 >/dev/null &&
python k-means/create_data.py 1000000 k-means/1m.data &&
echo 'LOG: k-means data creation...good!' &&
mpirun -np 1 k-means/k_means 3 1000000 9 k-means/1m.data 10 &&
echo 'LOG: k-means work...good!' &&
make clean >/dev/null&&
echo 'LOG: Clean works'
