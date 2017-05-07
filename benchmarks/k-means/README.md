## HW-5 Mpi implementation of 1d_multi_grid

```
make
mpirun -np 4 ./multigrid_1d $((2**20)) 100 //5.250111 s
./multigrid_1d $((2**20)) 100 //7.033151 s
make clean
```


