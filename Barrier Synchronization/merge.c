#include <stdio.h>
#include "mpi.h"
#include <omp.h>	
#include <math.h>
#include<stdlib.h>
#include <string.h>

typedef struct flagsStruct{
	int* myFlags[2];
	int** partnerFlags[2];
} flags;


void central_barrier(int proc,int num_processes)
{
  int my_id, my_dst, my_src;
  int tag = 1;
  int my_msg[2];
  MPI_Status mpi_result;
  my_msg[0] = 0;
  my_msg[1] = 1;

  if(proc==0)
  {
   int i;
    for(i=0;i<num_processes-1;i++)
    {
    MPI_Recv(&my_msg, 2, MPI_INT,i+1, tag, MPI_COMM_WORLD, &mpi_result);
    }
    for(i=0;i<num_processes-1;i++)
    {
     MPI_Send(&my_msg, 2, MPI_INT, i+1, tag, MPI_COMM_WORLD);
    }
  }
 else
 {
  MPI_Send(&my_msg, 2, MPI_INT,0, tag, MPI_COMM_WORLD);
  MPI_Recv(&my_msg, 2, MPI_INT, 0, tag, MPI_COMM_WORLD, &mpi_result);

 }

}



void disseminationBarrier(int* sense, int* parity,flags* localFlags, int roundsPerBarrier)
{
	int iterator = 0;
	for(iterator=0; iterator<roundsPerBarrier; iterator++)
	{
		**(localFlags->partnerFlags[*parity]+iterator) = *sense;
		while(*(localFlags->myFlags[*parity]+iterator) != *sense);
	}
	if (*parity == 1)
		*sense = 1 - (*sense);
	*parity = 1 - (*parity);
}

int main(int argc, char **argv)
{
        struct timeval start, end;
	int numOfThreads = 0, numOfBarriers = 0, roundsPerBarrier = 0, itr1 = 0, itr2=0,itr3=0, my_id = -1, num_processes = -1, level=1;
	if(argc != 3)
	{
		printf("Usage: %s <num of threads> <num of barriers>\n", argv[0]);
		return 0;
	}
	MPI_Init(&argc, &argv);
  	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
  	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	numOfThreads = atoi(argv[1]);
	numOfBarriers = atoi(argv[2]);
	roundsPerBarrier = ceil(log(numOfThreads)/log(2));	
	printf("[START] Process Id: %d\n", my_id);		
	omp_set_num_threads(numOfThreads);
	flags* allNodes = (flags*)malloc(sizeof(flags)*numOfThreads);
	if(!allNodes)
	{
		printf("Allocation of memory to allNodes failed.\n");
		return 1;
	}
	for(itr1 = 0; itr1<numOfThreads; itr1++) 
	{
		for(itr2=0;itr2<2;itr2++)
		{
                	allNodes[itr1].myFlags[itr2] = (int*)malloc(sizeof(int)*roundsPerBarrier);
			allNodes[itr1].partnerFlags[itr2] = (int**) malloc(sizeof (int*)*roundsPerBarrier);
			memset(allNodes[itr1].myFlags[itr2], 0, roundsPerBarrier);
		}
	}
	
	for(itr1=0;itr1<numOfThreads;itr1++)
		for(itr2=0;itr2<2;itr2++)
			for(itr3=0;itr3<roundsPerBarrier;itr3++)
				*(allNodes[itr1].partnerFlags[itr2]+itr3) = allNodes[(int)(itr1+pow(2,itr3))%numOfThreads].myFlags[itr2]+itr3;

         gettimeofday(&start, NULL);	
	for(itr1=0; itr1<numOfBarriers; itr1++)
	{	
		#pragma omp parallel shared(allNodes,numOfThreads, numOfBarriers, roundsPerBarrier, my_id)
		{
			int parity = 0, sense =1, barrierItr=0, fakeItr=0;
			int numThreads = omp_get_num_threads();
			int threadNum = omp_get_thread_num();
			flags* localFlags = &allNodes[threadNum];
			for(barrierItr=0; barrierItr<numOfBarriers; barrierItr++)
			{	
				for(fakeItr=0;fakeItr<1000;fakeItr++);
				printf("THREAD BARRIER START: Thread %d with Process Id %d\n", threadNum, my_id);
				disseminationBarrier(&sense, &parity, localFlags, roundsPerBarrier);
				printf("THREAD BARRIER FINISH: Thread %d with Process Id %d\n", threadNum, my_id);
			}				
		}
		central_barrier(my_id,num_processes);
		printf("[FINISH] Process %d.\n", my_id);
	}	
          gettimeofday(&end, NULL);
        printf("TIME TAKEN %ld  \n",((end.tv_sec * 1000000 + end.tv_usec)
		  - (start.tv_sec * 1000000 + start.tv_usec)));
  	MPI_Finalize();
  	return 0;
}
