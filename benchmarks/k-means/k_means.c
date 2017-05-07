#include <stdio.h>
#include <math.h>
#include "util.h"
#include <string.h>
#include <mpi.h>
int mpirank, p;
int pN,d,C,N;

// from hpc2017 class started code
static int compare(const void *a, const void *b)
{
  int *da = (int *)a;
  int *db = (int *)b;

  if (*da > *db)
    return 1;
  else if (*da < *db)
    return -1;
  else
    return 0;
}

/* compute residual vector */
void compute_residual(double *u, double *rhs, double *res, int N, double invhsq)
{
  int i;
   double ll,rr;
  MPI_Status status, status1;

  if (mpirank < p - 1) {
      /* If not the last process, send/recv bdry values to the right */
      MPI_Send(&u[N-1], 1, MPI_DOUBLE, mpirank+1, 124, MPI_COMM_WORLD);
      MPI_Recv(&rr, 1, MPI_DOUBLE, mpirank+1, 123, MPI_COMM_WORLD, &status);
    }
  if (mpirank > 0) {
      /* If not the first process, send/recv bdry values to the left */
      MPI_Send(&u[1], 1, MPI_DOUBLE, mpirank-1, 123, MPI_COMM_WORLD);
      MPI_Recv(&ll, 1, MPI_DOUBLE, mpirank-1, 124, MPI_COMM_WORLD, &status1);
    }

  for (i = 1; i < N; i++)
    res[i] = (rhs[i] - (2.*u[i] - u[i-1] - u[i+1]) * invhsq);

   if (mpirank > 0) {
    //get uf[-1]
    res[0] = (rhs[0] - (2.*u[0] - ll - u[1]) * invhsq);
  }
   if (mpirank  < p - 1) {
    //get -2 uf[N+1]
    res[N] = (rhs[N] - (2.*u[N] - u[N-1] - rr) * invhsq);
    
  }

}
/* Perform Jacobi iterations on u */
void jacobi(double *lu, double *rhs, int lN, double hsq, int ssteps)
{
  int i, j;
  double ll,rr;
  MPI_Status status;
  MPI_Request request_out1, request_in1;
  MPI_Request request_out2, request_in2;

  /* Jacobi damping parameter -- plays an important role in MG */
  double omega = 2./3.;
  double *lunew = calloc(sizeof(double), lN+2);
  for (j = 0; j < ssteps; ++j) {

    if (mpirank < p - 1) {
      /* If not the last process, send/recv bdry values to the right */
      MPI_Irecv(&rr, 1, MPI_DOUBLE, mpirank+1, 123, MPI_COMM_WORLD, &request_in1);
      MPI_Isend(&(lu[lN-1]), 1, MPI_DOUBLE, mpirank+1, 124, MPI_COMM_WORLD, &request_out1);
    }
    if (mpirank > 0) {
      /* If not the first process, send/recv bdry values to the left */
      MPI_Irecv(&ll, 1, MPI_DOUBLE, mpirank-1, 124, MPI_COMM_WORLD, &request_in2);
      MPI_Isend(&(lu[1]), 1, MPI_DOUBLE, mpirank-1, 123, MPI_COMM_WORLD, &request_out2);
    }

    /* Jacobi step for all the inner points */
    for (i = 1; i < lN; i++){
      lunew[i]  = lu[i] +  omega * 0.5 * (hsq*rhs[i] + lu[i-1] + lu[i+1] - 2*lu[i]);
    }

    /* check if Isend/Irecv are done */
    if (mpirank < p - 1) {
      MPI_Wait(&request_out1, &status);
      MPI_Wait(&request_in1, &status);
      lunew[lN]  = lu[lN] +  omega * 0.5 * (hsq*rhs[lN] + lu[lN-1] + rr - 2*lu[lN]);
    }
    if (mpirank > 0) {
      MPI_Wait(&request_out2, &status);
      MPI_Wait(&request_in2, &status);
      lunew[0]  = lu[0] +  omega * 0.5 * (hsq*rhs[0] + ll + lu[1] - 2*lu[0]);
    }
     memcpy(lu, lunew, (lN+1)*sizeof(double));
  }
//  output_to_screen(lu,lN);
  free (lunew);
}

double **alloc_2d_double(int rows, int cols) {
  // modified from http://stackoverflow.com/questions/5901476/sending-and-receiving-2d-array-over-mpi
    double *data = (double *)malloc(rows*cols*sizeof(double));
    double **array= (double **)malloc(rows*sizeof(double*));
    for (int i=0; i<rows; i++)
        array[i] = &(data[cols*i]);
    return array;
}


void printData(double **data,row,col){
  int k,j;
  for (k = 0; k < row ; k++)
      {
          for (j = 0; j<col; j++)
          {
              printf("%f ", data[k][j]);
          }
          printf("\n");
      }
}


void readDataPartial(char *file_name,double **data){
    int start = mpirank*pN;
    int end = (mpirank+1)*pN;
    printf("Rank %d range, %d-%d\n",mpirank,start,end);
    FILE *fp = fopen(file_name, "r");
    const char s[1] = ",";
    char *token;
    int i;
    int line_count=0;
    int read_count=0;
    if(fp != NULL)
    {
        char line[100];
        while(fgets(line, sizeof line, fp) != NULL)
        {
            if (line_count>=start && line_count<end){
                token = strtok(line, s);
                for(i=0;i<d;i++)
                {
                    data[read_count][i] = atof(token);
                    if(i<d-1)
                    {   
                        token = strtok(NULL,s);
                    }    
                }
                read_count++;
            }
            line_count++;
        }
        fclose(fp);
    } else {
        perror(file_name);
    }  
}

void getInitialClustersFromData(char *file_name,double **centers){
    int indices[C];
    FILE *fp = fopen(file_name, "r");
    const char s[1] = ",";
    char *token;
    int i;
    int line_count=0;
    int prev_i = -1;
    int c_i = 0;
    for (i = 0; i < C ; i++){
      indices[i]=rand() % N;
       // printf("C%d is %d\n",i,indices[i]);
    }
    qsort(indices, C, sizeof(int), compare);
    if(fp != NULL)
    {
        char line[100];
        while(fgets(line, sizeof line, fp) != NULL)
        {
            if (line_count==indices[c_i]){
                printf("Center %d from line %d\n",c_i,line_count);
                token = strtok(line, s);
                for(i=0;i<d;i++)
                {
                    centers[c_i][i] = atof(token);
                    if(i<d-1)
                    {   
                        token = strtok(NULL,s);
                    }    
                }
                c_i++;

                if (c_i == C){
                  break;
                }
                //simple duplicate index check .TODO make this at the beginning
                else if (indices[c_i]==indices[c_i-1]){
                    indices[c_i]+=1;
                }
                    
            }
            line_count++;
        }
        fclose(fp);
    } else {
        perror(file_name);
    }  
}


int main(int argc, char * argv[])
{
// GLOBAL  int mpirank, p;
// GLOBAL int pN,d,C,N;
  int i,j,k,l;
  char f_name[100];
  int N_ITER;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpirank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
    /* get name of host running MPI process */
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);
  printf("Rank %d/%d running on %s.\n", mpirank, p, processor_name);



  if (argc != 6 && mpirank == 0 ) {
    fprintf(stderr, "Usage: ./k_means dim N C data n_iter\n");
    fprintf(stderr, "dim: data dimension\n");
    fprintf(stderr, "N: # data points\n");
    fprintf(stderr, "C: number of clusters\n");
    fprintf(stderr, "data: data file path\n");
    fprintf(stderr, "n_iter: number of iterations\n");
    MPI_Abort(MPI_COMM_WORLD, 0);
  }

  //READ arguments
  sscanf(argv[1], "%d", &d);
  sscanf(argv[2], "%d", &N);
  sscanf(argv[3], "%d", &C);
  sscanf(argv[5], "%d", &N_ITER);

  if (((N % p)!=0) && mpirank == 0 ) {
    printf("N: %d\n", N);
    printf("p: %d\n", p);
    printf("Exiting. N must be a power of p. Accepting this is trivial, but not implemented/tested.\n");
    MPI_Abort(MPI_COMM_WORLD, 0);
  }
  pN = N / p;
  strcpy(f_name, argv[4]);
  if (mpirank==0) printf("%dd data,Total: %d points,%d points per process from file %s\n",d,N,pN,f_name);

  double **centers;
  double **class_data_sum;
  int local_class_residual_count[C];
  int global_class_residual_count[C];
  centers = alloc_2d_double(C,d);
  class_data_sum = alloc_2d_double(C,d);

  if (mpirank==0){
    int i;

    getInitialClustersFromData(f_name,centers);
  }
  MPI_Bcast(&(centers[0][0]), C*d, MPI_DOUBLE,0,MPI_COMM_WORLD);

  double min_res,c_res;
  int min_i;

  /* timing */
  double     start, end;
  MPI_Barrier(MPI_COMM_WORLD);
  start = MPI_Wtime();

  double **data;
  data = alloc_2d_double(pN,d);
  readDataPartial(f_name,data);
  for (i=0; i < N_ITER; i++){
    //For each turn zero the sums

    for (k=0; k<C; k++){
      for (j=0;j<d;j++){
        class_data_sum[k][j]=0;
      }
      local_class_residual_count[k]=0;
    }

    //For each data point get the clusters cluster center and add the residual
    for (j=0; j< pN; j++ ){
      min_res = INFINITY;
      min_i = -1;

      for (k=0; k<C; k++){
        c_res=0.0;

        //norm
        for (l=0;l<d;l++){
          c_res += pow(abs(data[j][l]-centers[k][l]),2);
        }
        c_res = sqrt(c_res);

        if (c_res<min_res){
          min_i = k;
          min_res = c_res;
        }

      }

     for (l=0; l<d; l++){
        class_data_sum[min_i][l] += data[j][l];
     }
     local_class_residual_count[min_i] += 1;
    }

    //Share the sums of points and the countrs
    MPI_Allreduce(&(class_data_sum[0][0]),&(centers[0][0]), C*d, MPI_DOUBLE, MPI_SUM,
              MPI_COMM_WORLD);
    MPI_Allreduce(local_class_residual_count,global_class_residual_count, C, MPI_INT, MPI_SUM,
              MPI_COMM_WORLD);

   for (k=0; k<C; k++){
        for (l=0;l<d;l++){
          centers[k][l] /= global_class_residual_count[k];
        }
    }
    // if (mpirank==1){
    //   printf("rank1: centers\n");
    //   printData(centers,C,d);
    // }
  }
  // if (mpirank==1){
  //     printf("rank1:\n");
  //     printData(centers,C,d);
  // }
    if (mpirank==0){
      printf("centers:\n");
      printData(centers,C,d);
    }

  free(data[0]);
  free(data);
  free(centers[0]);
  free(centers);
  /* timing */
  MPI_Barrier(MPI_COMM_WORLD);
  end = MPI_Wtime();

  if (0 == mpirank) {
    printf("Time elapsed is %f seconds.\n", end-start);
  }
  MPI_Finalize();
  return 0;
}
