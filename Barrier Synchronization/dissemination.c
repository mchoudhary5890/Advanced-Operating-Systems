#include <stdio.h>
#include <omp.h>
#include <math.h>
#include<stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
typedef struct flagsStruct{
	int* myFlags[2];
	int** partnerFlags[2];
} flags;

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

int main(int argc, char* argv[])
{
	int numOfThreads = 0, numOfBarriers = 0, roundsPerBarrier = 0, itr1 = 0, itr2=0,itr3=0;
	if(argc != 3)
	{
		printf("Usage: %s <num of threads> <num of barriers>\n", argv[0]);
		return 0;
	}
	numOfThreads = atoi(argv[1]);
	numOfBarriers = atoi(argv[2]);
	roundsPerBarrier = ceil(log(numOfThreads)/log(2));	
	printf("Rounds per barrier: %d\n", roundsPerBarrier);	
	omp_set_num_threads(numOfThreads);
	flags* allNodes = (flags*)malloc(sizeof(flags)*numOfThreads);
	struct timeval start, end;
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
	#pragma omp parallel shared(allNodes,numOfThreads, numOfBarriers, roundsPerBarrier) 
	{
		int parity = 0, sense =1, barrierItr=0, fakeItr;
		int threadNum = omp_get_thread_num();
		int numThreads = omp_get_num_threads();
		flags* localFlags = &allNodes[threadNum];
		for(barrierItr=0; barrierItr<numOfBarriers; barrierItr++)
		{	
			for(fakeItr=0;fakeItr<1000;fakeItr++);
			printf("BARRIER START: Thread %d with total threads %d\n", threadNum, numThreads);
			disseminationBarrier(&sense, &parity, localFlags, roundsPerBarrier);
			printf("BARRIER FINISH: Thread %d with total threads %d\n", threadNum, numThreads);
		}			
	}
	gettimeofday(&end, NULL);
	printf("%ld\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
	return 0;
}
