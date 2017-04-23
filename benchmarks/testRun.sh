make all >/dev/null&&
echo 'LOG: Successfully compiles' &&
mpirun -np 10 ssort/ssort 100 ssort/ >/dev/null&&
echo 'LOG: ssort...good!'&&
mpirun -np 16 jacobi-2D/jacobi-mpi2D-nonBlocking 100 100 >/dev/null &&
echo 'LOG: jacobi-2D...good!' &&
make clean >/dev/null&&
echo 'LOG: Clean works'
