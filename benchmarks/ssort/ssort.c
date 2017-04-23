/* Parallel sample sort
 */
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <mpi.h>
#include <stdlib.h>


void pArray(int *arr,int N,int rank){
  int row;
  printf("Rank %d: ",rank);
  for (row=0; row<N; row++)
  {
     printf("%d     ", arr[row]);
   }
   printf("\n");
}

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

int main( int argc, char *argv[])
{
  int rank;
  int i, N ,P;
  int *vec;
  int MASTER = 0 ;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &P);
  /* timing */
  double     start, end;
  MPI_Barrier(MPI_COMM_WORLD);
  start = MPI_Wtime();

  /* Number of random numbers per processor (this should be increased
   * for actual tests or could be passed in through the command line */
  sscanf(argv[1], "%d", &N);
  char *FOLDER_PATH = argv[2];

  vec = calloc(N, sizeof(int));
  /* seed random number generator differently on every core */
  srand((unsigned int) (rank + 393919));

  /* fill vector with random integers */
  for (i = 0; i < N; ++i) {
    vec[i] = rand();
  }
 // printf("rank: %d, first entry: %d\n", rank, vec[0]);

  /* sort locally */
  qsort(vec, N, sizeof(int), compare);
  //if(rank==1)
    //pArray(vec,N,rank);
  /* randomly sample s entries from vector or select local splitters,
   * i.e., every N/P-th entry of the sorted vector */
    MPI_Datatype sampler;
    MPI_Type_vector((int) N/P, 1, P , MPI_INT,&sampler);
    MPI_Type_commit(&sampler);
 
  /* every processor communicates the selected entries
   * to the root processor; use for instance an MPI_Gather */
   int *samples = NULL;
  int S_N;
  int S =(int) N/P;
  if(MASTER==rank){
    printf("N: %d, P: %d\n",N,P);
    S_N = S*P;
    samples = malloc(sizeof(int)*S_N);
  }
  int *displs = malloc(P*sizeof(int)); 
  int *rcounts = malloc(P*sizeof(int)); 
  for (i=0; i<P; ++i) { 
      displs[i] = i*S; 
      rcounts[i] = (int) N/P; 
  } 
  MPI_Gatherv(vec, 1, sampler, samples ,rcounts,displs, MPI_INT, MASTER, MPI_COMM_WORLD);
  //if(rank==MASTER)
    //pArray(samples,S_N,rank);
  /* root processor does a sort, determinates splitters that
   * split the data into P buckets of approximately the same size */
  // I will choose P-1 points from the ordered samples. 
  // For example. N=10, P=2 -> S_N=10,  S=5 , S2=10. And I would sample samples[j] for j=5
  // For example. N=10, P=3 -> S_N=9,  S=3 , S2=4. And I would sample samples[j] for j=2,6: which are the rigt pevots 0 1 _2_ 3 4 5 _6_ 7 8

  int *buckets = malloc((P+1)*sizeof(int));
  if(rank==MASTER){
      qsort(samples, S_N, sizeof(int), compare);
      //pArray(samples,S_N,rank);
      buckets[0]=-INT_MAX;
      buckets[P]=INT_MAX;
      for (i=1; i<P; ++i) { 
        //finding best ones. Find this formula emprically by trying out different values.
        int j = i*S+1;
        buckets[i] = samples[j];
      }
  }
   
  /* root process broadcasts splitters */
  int *out_counts = malloc((P)*sizeof(int));
  int *in_counts = malloc((P)*sizeof(int));
  MPI_Bcast(buckets,P+1,MPI_INT,MASTER,MPI_COMM_WORLD);
  //pArray(buckets,P+1,rank);

  /* every processor uses the obtained splitters to decide
   * which integers need to be sent to which other processor (local bins) */
  int b;
  int prev=0;
  i = 0;
  for (b = 0; b < P; b++) {
    while (vec[i]<= buckets[b+1] && vec[i] > buckets[b])
      i++;
    out_counts[b] = i-prev;
    prev=i;
  }
  //pArray(out_counts,P,rank);
  MPI_Alltoall(out_counts, 1, MPI_INT, in_counts, 1, MPI_INT, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
  //pArray(in_counts,P,rank);
  /* send and receive: either you use MPI_AlltoallV, or
   * (and that might be easier), use an MPI_Alltoall to share
   * with every processor how many integers it should expect,
   * and then use MPI_Send and MPI_Recv to exchange the data */
  MPI_Request request_out[P], request_in[P];
  MPI_Status status;
  int total=0;
  for (b = 0; b < P; b++) {
    total += in_counts[b];
  }
  int *result_a = malloc(total*sizeof(int));
  int sum_in = 0;
  int sum_out = 0;
  for (b = 0; b < P; b++) {
    MPI_Isend(&(vec[sum_out]), out_counts[b], MPI_INT, b, 1, MPI_COMM_WORLD,&request_out[b]);
    sum_out += out_counts[b];
    MPI_Irecv(&(result_a[sum_in]), in_counts[b], MPI_INT, b, 1, MPI_COMM_WORLD, &request_in[b]);
    sum_in += in_counts[b];
  }
   for (b = 0; b < P; b++) {
    MPI_Wait(&request_out[b], &status);
    MPI_Wait(&request_in[b], &status);
  } 

  /* do a local sort */
  qsort(result_a, total, sizeof(int), compare);

  /* every processor writes its result to a file */
    /*
   * Write output to a file
   */
    /* timing */
  MPI_Barrier(MPI_COMM_WORLD);
  end = MPI_Wtime();

  if (MASTER == rank) {
    printf("Time elapsed is %f seconds.\n", end-start);
  }

  {
    FILE* fd = NULL;
    char filename[256];
    snprintf(filename, 256, "%soutput%02d.txt",FOLDER_PATH,rank);
    fd = fopen(filename,"w+");

    if(NULL == fd)
    {
      printf("Error opening file \n");
      return 1;
    }
    for(i = 0; i < total; ++i)
      fprintf(fd, "%d\n", result_a[i]);

    fclose(fd);
  }

  free(out_counts);
  free(in_counts);
  free(buckets);
  free(samples);
  free(displs);
  free(rcounts);
  free(vec);
  MPI_Finalize();
  return 0;
}