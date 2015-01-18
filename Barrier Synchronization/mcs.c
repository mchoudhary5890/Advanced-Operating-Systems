#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
// to compile : mpicc -o mcs mcs.c -lm
// To run: mpirun -np 4 mcs 4

typedef struct treenode{
	int childPointers[2];
	int haveChild[4];
} treeNode;

void mellCrumScottBarrier(treeNode* nodePtr, int processRank)
{
	int msgBuffer = 0, iterator = 0, msgTag = 5890;
	MPI_Status status;
	for(iterator=0;iterator<4;iterator++)
	{
		if(nodePtr->haveChild[iterator] == 1)
		{
			MPI_Recv(&msgBuffer, 1, MPI_INT, (4*processRank+iterator+1), msgTag, MPI_COMM_WORLD, &status);
		}
	}
	if(processRank)
	{
		MPI_Send(&msgBuffer, 1, MPI_INT, floor((processRank-1)/4), msgTag, MPI_COMM_WORLD);
		MPI_Recv(&msgBuffer, 1, MPI_INT, floor((processRank-1)/2), msgTag, MPI_COMM_WORLD, &status);
	}
	for(iterator=0;iterator<2;iterator++)
	{	
		if(nodePtr->childPointers[iterator]!= -1)
			MPI_Send(&msgBuffer, 1, MPI_INT, nodePtr->childPointers[iterator], msgTag, MPI_COMM_WORLD);
	}
}

int main(int argc, char* argv[])
{
	int numOfBarriers=0, processRank = -1, numOfProcesses = 0, iterator = 0, fakeItr = 0;
	treeNode* nodePtr = NULL;
	if(argc !=2)
	{
		printf("Usage: %s <numOfBarriers>\n", argv[0]);
		return 0;
	}
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);	
	MPI_Comm_rank(MPI_COMM_WORLD, &processRank);
	printf("Process Id: %d\n", processRank);
	numOfBarriers = atoi(argv[1]);
	struct timeval start, end;
	if(!numOfProcesses)
	{
		printf("No. of Processes is 0. Exiting from %d.\n", processRank);
		return 1;
	}
	nodePtr = (treeNode*)malloc(sizeof(treeNode));
	if(!nodePtr)
	{
		printf("Failed while allocating memory for nodePtr. Exiting from %d.\n", processRank);
		return 1;
	}
	for(iterator = 0; iterator<4;iterator++)
	{
		if(4*processRank+iterator+1 < numOfProcesses)
			nodePtr->haveChild[iterator] = 1;
		else
			nodePtr->haveChild[iterator] = 0;
	} 	
	nodePtr->childPointers[0] = ((2*processRank+1) >= numOfProcesses)? -1 : (2*processRank+1);
	nodePtr->childPointers[1] = ((2*processRank+2) >= numOfProcesses)? -1 : (2*processRank+2);
	gettimeofday(&start, NULL);	
	for(iterator=0; iterator<numOfBarriers; iterator++)
	{	
		for(fakeItr=0;fakeItr<1000;fakeItr++);
		printf("BARRIER START: Process %d with total Processes %d\n", processRank, numOfProcesses);
		mellCrumScottBarrier(nodePtr, processRank);
		printf("BARRIER FINISH: Process %d with total Processes %d\n", processRank, numOfProcesses);
	}
	gettimeofday(&end, NULL);
	printf("%ld\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
	MPI_Finalize();
	return 0;
}
