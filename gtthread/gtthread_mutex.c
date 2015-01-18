#include "gtthread.h"
#include "shared.h"
mutexListPtr globalMutexList = NULL;
gtthread_mutex_t maxMutexId = 1;

static mutexListNode* createMutexNode()
{
	mutexListNode* newMutexNode = (mutexListNode*)malloc(sizeof(mutexListNode));
	if(!newMutexNode)
	{
		printf("createMutexNode(): FAILED\n");
		return NULL;
	}
	newMutexNode->mutexId = maxMutexId++;
	newMutexNode->isLocked = 0;
	newMutexNode->currentOwnerThread = NULL;
	newMutexNode->mutexWaitingList = NULL;
	newMutexNode->next = NULL;
	return newMutexNode;
}

static mutexListNode* lookUpMutexNode(gtthread_mutex_t mutexId)
{
	mutexListNode* mutexToReturn = globalMutexList; 
	while(mutexToReturn && (mutexToReturn->mutexId != mutexId))
		mutexToReturn = mutexToReturn->next;
	return mutexToReturn;				
}

static mutexWaitingListNode* dequeueNextOwner(mutexWaitingListPtr* mutexWaitingList) 
{
	mutexWaitingListNode* nodeToReturn;
	if((*mutexWaitingList)->next == (*mutexWaitingList))
	{
		nodeToReturn = *mutexWaitingList;
		*mutexWaitingList = NULL;
	}
	else
	{
		nodeToReturn = (*mutexWaitingList)->next;
		(*mutexWaitingList)->next = ((*mutexWaitingList)->next)->next;
	}
	return nodeToReturn;
}


static void enqueueMutexWaitingList(mutexWaitingListPtr* mutexWaitingList, threadListNode* nodeToInsert)
{
	mutexWaitingListNode* newWaitingNode = NULL;
	if(!nodeToInsert)
	{
		printf("enqueueMutexWaitingList(): FAILED - nodeToInsert is empty\n");
		return;
	}
	newWaitingNode = (mutexWaitingListNode*)malloc(sizeof(mutexWaitingListNode));
	if(!newWaitingNode)
	{
		printf("enqueueMutexWaitingList(): FAILED\n");
		return;
	}
	newWaitingNode->threadPtr = nodeToInsert;	
	if(!(*mutexWaitingList))
	{
		*mutexWaitingList = newWaitingNode;
		(*mutexWaitingList)->next = *mutexWaitingList;
		return;
	}
	newWaitingNode->next = (*mutexWaitingList)->next;
        (*mutexWaitingList)->next = newWaitingNode;
	*mutexWaitingList = newWaitingNode;
}

int  gtthread_mutex_init(gtthread_mutex_t* mutex)
{
	mutexListNode* newMutexNode = NULL;
	sigprocmask(SIG_BLOCK, &signalMask, NULL);
	newMutexNode = createMutexNode();
	if(!newMutexNode)
	{
		printf("gtthread_mutex_init(): FAILED to create new mutex.\n");
		sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
		return -1;
	}
	*mutex = newMutexNode->mutexId;
	newMutexNode->next = globalMutexList;
	globalMutexList = newMutexNode;
	sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
	return 0;
}

int  gtthread_mutex_lock(gtthread_mutex_t *mutex)
{
	mutexListNode* mutexNode = NULL;
	sigprocmask(SIG_BLOCK, &signalMask, NULL);
	if((!mutex) || (!globalMutexList))
	{	
		sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
		printf("gtthread_mutex_lock(): INVALID ARGUMENT\n");
		return -1;
	}
	mutexNode = lookUpMutexNode(*mutex);
	if(!mutexNode)
	{
		sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
		printf("gtthread_mutex_lock(): Mutex not initialized.\n");
		return -1;
	}
	if(!(mutexNode->isLocked))
	{
		mutexNode->isLocked = 1;
		mutexNode->currentOwnerThread =  currentThread;
		sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
		return 0;
	}
	enqueueMutexWaitingList(&(mutexNode->mutexWaitingList), currentThread);
	currentThread->isWaiting = 1;
	enqueue(&waitingThreadQueue, currentThread);
	sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
	raise(SIGNAL_TYPE);
	return 0;
}	

int  gtthread_mutex_unlock(gtthread_mutex_t *mutex)
{
	threadListNode* tempThread = NULL;
	mutexListNode* mutexNode = NULL;
	mutexWaitingListNode* nextMutexOwner = NULL;
	sigprocmask(SIG_BLOCK, &signalMask, NULL);
	if((!mutex) || (!globalMutexList))
	{	
		sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
		printf("gtthread_mutex_unlock(): INVALID ARGUMENT\n");
		return -1;
	}	
	mutexNode = lookUpMutexNode(*mutex);
	if(!mutexNode)
	{	
		sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
		printf("gtthread_mutex_unlock(): mutex not initialized\n");
		return -1;
	}
	if(!(mutexNode->isLocked))
	{	
		sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
		printf("gtthread_mutex_unlock(): mutex is not locked.\n");
		return -1;
	}			
	if((mutexNode->currentOwnerThread)!= currentThread)
	{
		sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
		printf("gtthread_mutex_unlock(): Caller doesn't own the mutex.\n");
		return -1;
	}		
	if(!(mutexNode->mutexWaitingList))
	{
		mutexNode->isLocked = 0;
		mutexNode->currentOwnerThread = NULL;
		sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
		return 0;
	}		
	nextMutexOwner = dequeueNextOwner(&(mutexNode->mutexWaitingList)); 
	mutexNode->currentOwnerThread = nextMutexOwner->threadPtr;
	tempThread = dequeueByThreadId(&waitingThreadQueue, (gtthread_t)(nextMutexOwner->threadPtr)->threadId);
	tempThread->isWaiting = 0;
	enqueue(&readyThreadQueue, tempThread);
	free(nextMutexOwner);
	sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
	return 0;
}
