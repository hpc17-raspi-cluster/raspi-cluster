/* MPI-parallel Jacobi smoothing to solve -u''=f
 * Global vector has N unknowns, each processor works with its
 * part, which has lN = N/p unknowns.
 * Author: Georg Stadler
 */
#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <string.h>
#include <stdlib.h>

// http://www.geeksforgeeks.org/find-whether-a-given-number-is-a-power-of-4-or-not/
void pMatrix(double **matrix,int lN){
  int row, columns;
  for (row=0; row<lN+2; row++)
  {
      for(columns=0; columns<lN+2; columns++)
           printf("%f     ", matrix[row][columns]);
      printf("\n");
   }
   printf("\n");
}

// http://www.geeksforgeeks.org/find-whether-a-given-number-is-a-power-of-4-or-not/
int isPowerOfFour(int n)
{
  if(n == 0)
    return 0;
  while(n != 1)
  {    
   if(n%4 != 0)
      return 0;
    n = n/4;      
  }
  return 1;
} 
/* compuate global residual, assuming ghost values are updated */
double compute_residual(double **lu, int lN, double invhsq)
{
  int i,j;
  double tmp, gres = 0.0, lres = 0.0;

  for (i = 1; i <= lN; i++){
    for (j = 1; j <= lN; j++){
    tmp = ((4.0*lu[i][j] - lu[i-1][j] - lu[i+1][j]- lu[i][j-1]- lu[i][j+1]) * invhsq - 1);
    lres += tmp * tmp;
  }
}
  /* use allreduce for convenience; a reduce would also be sufficient */
  MPI_Allreduce(&lres, &gres, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  return sqrt(gres);
}


int main(int argc, char * argv[])
{
  int mpirank, i,j, p, lN,N, iter, max_iters,ppr;
  MPI_Status status, status1;
  MPI_Request request_out1, request_in1;
  MPI_Request request_out2, request_in2;
  MPI_Request request_out3, request_in3;
  MPI_Request request_out4, request_in4;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpirank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  /* get name of host running MPI process */
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);
  printf("Rank %d/%d running on %s.\n", mpirank, p, processor_name);

  sscanf(argv[1], "%d", &N);
  sscanf(argv[2], "%d", &max_iters);

  ppr =(int) sqrt(p);
  if ((!isPowerOfFour(p) || (N % ppr)!=0) && mpirank == 0 ) {
    printf("N: %d\n", N);
    printf("p: %d\n", p);
    printf("Exiting. p must be a power of 4 and N being divisible by sqrt(p).\n");
    MPI_Abort(MPI_COMM_WORLD, 0);
  }
  lN = N/ppr;

  MPI_Datatype col_type;
  MPI_Type_vector(lN+2, 1, lN+2, MPI_DOUBLE,&col_type);
    MPI_Type_commit(&col_type);

  /* timing */
  double     start, end;
  MPI_Barrier(MPI_COMM_WORLD);
  start = MPI_Wtime();

  /* Allocation of vectors, including left and right ghost points */
  //No zeroing!
  double **lu;
  double **lunew;
  double **lutemp;
  lunew = calloc(lN+2, sizeof(double *));
  lu = calloc(lN+2, sizeof(double *));
  lunew[0] = calloc((lN+2)*(lN+2), sizeof(double));
  lu[0] = calloc((lN+2)*(lN+2), sizeof(double));
  //continious allocation
  for (i=0; i<(lN+2); i++)
  {
    lu[i] = &(lu[0][i*(lN+2)]);
    lunew[i] = &(lunew[0][i*(lN+2)]);
  }

  //We checked earlier that p is a power of 4 meaning it is a perfect square.

  double h = 1.0 / (N + 1);
  double hsq = h * h;
  double invhsq = 1./hsq;
  double gres, gres0, tol = 1e-5;
 
  /* initial residual */
  gres0 = compute_residual(lu, lN, invhsq);
  gres = gres0;

  for (iter = 0; iter < max_iters && gres/gres0 > tol; iter++) {

        /* Jacobi step for local points */
    for (i = 1; i <= lN; i++){
         lunew[1][i]  = 0.25 * (hsq + lu[0][i] + lu[2][i]+ lu[1][i-1]+ lu[1][i+1]);
         lunew[lN][i]  = 0.25 * (hsq + lu[lN-1][i] + lu[lN+1][i]+ lu[lN][i-1]+ lu[lN][i+1]);
         lunew[i][1]  = 0.25 * (hsq + lu[i-1][1] + lu[i+1][1]+ lu[i][0]+ lu[i][2]);
         lunew[i][lN]  = 0.25 * (hsq + lu[i-1][lN] + lu[i+1][lN]+ lu[i][lN-1]+ lu[i][lN+1]);
    }

    /* communicate ghost values */
    if (mpirank%ppr < ppr - 1) {
      /* If not the last process, send/recv bdry values to the right */
      MPI_Isend(&(lunew[0][lN]), 1, col_type, mpirank+1, 1, MPI_COMM_WORLD,&request_out1);
      MPI_Irecv(&(lunew[0][lN+1]), 1, col_type, mpirank+1, 2, MPI_COMM_WORLD, &request_in1);
    }
    if (mpirank%ppr > 0) {
      /* If not the first process, send/recv bdry values to the left */
      MPI_Irecv(&(lunew[0][0]), 1, col_type, mpirank-1, 1, MPI_COMM_WORLD, &request_in2);
      MPI_Isend(&(lunew[0][1]), 1, col_type, mpirank-1, 2, MPI_COMM_WORLD,&request_out2);
    }
    
    if (mpirank<(p-ppr)) {
      /* If not the last process, send/recv bdry values to the right */
      MPI_Isend(lunew[1], lN+2, MPI_DOUBLE, mpirank+ppr, 1, MPI_COMM_WORLD,&request_out3);
      MPI_Irecv(lunew[0], lN+2, MPI_DOUBLE, mpirank+ppr, 2, MPI_COMM_WORLD, &request_in3);
    }
    if (mpirank >=ppr ) {
      /* If not the first process, send/recv bdry values to the left */
      MPI_Irecv(lunew[lN+1], lN+2, MPI_DOUBLE, mpirank-ppr, 1, MPI_COMM_WORLD, &request_in4);
      MPI_Isend(lunew[lN], lN+2, MPI_DOUBLE, mpirank-ppr, 2, MPI_COMM_WORLD, &request_out4);
    }

    /* Jacobi step for local points */
    for (i = 2; i < lN; i++){
      for (j = 2; j < lN; j++){
         lunew[i][j]  = 0.25 * (hsq + lu[i-1][j] + lu[i+1][j]+ lu[i][j-1]+ lu[i][j+1]);
      }
    }


    /* communicate ghost values */
    if (mpirank%ppr < ppr - 1) {
      /* If not the last process, send/recv bdry values to the right */
      MPI_Wait(&request_out1, &status);
      MPI_Wait(&request_in1, &status);
    }
    if (mpirank%ppr > 0) {
      /* If not the first process, send/recv bdry values to the left */
      MPI_Wait(&request_out2, &status);
      MPI_Wait(&request_in2, &status);
    }
    
    if (mpirank<(p-ppr)) {
      /* If not the last process, send/recv bdry values to the right */
      MPI_Wait(&request_out3, &status);
      MPI_Wait(&request_in3, &status);
    }
    if (mpirank >=ppr ) {
      /* If not the first process, send/recv bdry values to the left */
      MPI_Wait(&request_out4, &status);
      MPI_Wait(&request_in4, &status);
    }


    /* copy newu to u using pointer flipping */
    lutemp = lu; lu = lunew; lunew = lutemp;
    if (0 == (iter % 10)) {
      gres = compute_residual(lu, lN, invhsq);
      if (0 == mpirank) {
  printf("Iter %d: Residual: %g\n", iter, gres);
      }
    }
  }

  /* Clean up */
free(lu[0]);
free(lu);
free(lunew[0]);
free(lunew);

  /* timing */
  MPI_Barrier(MPI_COMM_WORLD);
  end = MPI_Wtime();

  if (0 == mpirank) {
    printf("Time elapsed is %f seconds.\n", end-start);
  }
  MPI_Finalize();
  return 0;
}
