#include "gtthread.h"
#include "shared.h"
#define STACK_SIZE 8*1024
#define TIMER_TYPE ITIMER_VIRTUAL
#define TIME_ORDER 1000000
#define SIGNAL_TYPE SIGVTALRM

struct itimerval quantum;
struct sigaction signalToScheduler;
sigset_t signalMask;
gtthread_t maxThreadId = 1; 
threadListNode* currentThread = NULL;
threadListPtr readyThreadQueue = NULL;
threadListPtr terminatedThreadQueue = NULL;
threadListPtr waitingThreadQueue = NULL; 
threadListNode* dequeueByThreadId(threadListPtr*, gtthread_t);
	
static threadListNode* createThreadNode()
{
	threadListNode* newThreadNode = (threadListNode*)malloc(sizeof(threadListNode));

	if(!newThreadNode)
	{
		printf("createThreadNode(): FAILED\n");
		return NULL;
	}
	newThreadNode->threadId = maxThreadId++;
	newThreadNode->threadContext = NULL;
	newThreadNode->isMain = 0;
    	newThreadNode->isWaiting = 0; 	
	newThreadNode->next = NULL;
	newThreadNode->waitingThreadNode = NULL;
	return newThreadNode;
}	
	
threadListNode* dequeue(threadListPtr* readyThreadQueue)
{
   	threadListNode* returnNode = NULL;
	if(!(*readyThreadQueue))
	{
		return NULL;
	}
	if((*readyThreadQueue)->next == *readyThreadQueue)
        {
      		returnNode = *readyThreadQueue;
        	*readyThreadQueue = NULL;
    	}
	else
	{
		returnNode = (*readyThreadQueue)->next;
        	(*readyThreadQueue)->next = (*readyThreadQueue)->next->next;
    	}
	return returnNode;
}

void enqueue(threadListPtr* ThreadQueue, threadListNode* nodeToInsert)
{
	if(!nodeToInsert)
	{
		printf("enqueue(): FAILED - nodeToInsert is empty\n");
		return;
	}
	if(!(*ThreadQueue))
	{
		*ThreadQueue = nodeToInsert;
		(*ThreadQueue)->next = nodeToInsert;
		return;
	}
	nodeToInsert->next = (*ThreadQueue)->next;
        (*ThreadQueue)->next = nodeToInsert;
	*ThreadQueue = nodeToInsert;
}

void printQueue(threadListPtr queue)
{
	threadListPtr temp = NULL;
	if(!queue)
	{
		printf("Queue is empty\n");
	}
	else
	{
		temp = queue->next;
		do
		{
			printf("thread %d\t with next thread %d\n",(int)(temp->threadId), (int)((temp->next)->threadId));
			temp = temp->next;
		} while(temp!=(queue->next));
	}
}

threadListNode* dequeueByThreadId(threadListPtr* threadList, gtthread_t thread)
{
	threadListNode* threadToReturn = NULL;
	threadListNode* tempIterator = NULL;
	threadListNode* prevThreadNode = NULL;
	if(!(*threadList))
	{
		return NULL;
	}
	if((*threadList)->next == (*threadList))
	{
		if((*threadList)->threadId == thread)
		{
			threadToReturn = *threadList;
			*threadList = NULL;
		}
		else
		{
			threadToReturn = NULL;	
		}
	}
	else
	{
		tempIterator = (*threadList)->next;
		prevThreadNode = (*threadList);
		do
		{
			if(tempIterator->threadId == thread)
			{
				threadToReturn = tempIterator;
				prevThreadNode->next = tempIterator->next;
				if(tempIterator == (*threadList))	
					*threadList = prevThreadNode;	
				break;
			}
			prevThreadNode = tempIterator;
			tempIterator = tempIterator->next;
		}while(tempIterator != (*threadList)->next);
			
	}	
	threadToReturn->next = NULL;
	return threadToReturn;
}

void gtthread_init(long period)
{
	threadListNode* node = NULL;
	long sec = 0;
	long usec = 0;
	if(period <= 0)
	{
		printf("gtthread_init() : Wrong Period.\n");
		exit(1);
	}
	sec = period/TIME_ORDER;
	usec = period % TIME_ORDER;
	sigemptyset(&signalToScheduler.sa_mask);
	signalToScheduler.sa_flags = 0;
	signalToScheduler.sa_handler = scheduler;
	sigaction(SIGNAL_TYPE, &signalToScheduler, NULL);
	node = createThreadNode();
	if(!node)
	{
		printf("gtthread_init(): Error while creating main node.\n");
		exit(1);
	}
	node->threadContext = (ucontext_t*) malloc(sizeof(ucontext_t));
	if(!(node->threadContext))
	{
		free(node);
		printf("gtthread_init(): Error while allocating space for main node's context.\n");
		exit(1);
	}
	getcontext(node->threadContext);
	node->threadContext->uc_link = NULL;
	node->threadContext->uc_stack.ss_flags = 0;
	node->isMain  =	1;
	currentThread = node;
	sigemptyset(&signalMask);
	sigaddset(&signalMask,SIGNAL_TYPE);
	quantum.it_interval.tv_sec = sec;
	quantum.it_interval.tv_usec = usec;
	quantum.it_value.tv_sec = sec;
	quantum.it_value.tv_usec = usec;
	setitimer(TIMER_TYPE, &quantum, NULL);
}

void gtthread_exit(void *retval)
{
	threadListNode* tempWaitingThread = NULL;
	sigprocmask(SIG_BLOCK, &signalMask, NULL);
	currentThread->returnValue = retval;
	currentThread->hasTerminated = 1;	
	if(currentThread->waitingThreadNode)
	{	
		if((currentThread->waitingThreadNode)->joinStatus)
		{
			*((currentThread->waitingThreadNode)->joinStatus) = currentThread->returnValue;	
		}		
		(currentThread->waitingThreadNode)->isWaiting = 0;
		tempWaitingThread = dequeueByThreadId(&waitingThreadQueue, (currentThread->waitingThreadNode)->threadId);
		if(!tempWaitingThread) printf("dequeue by thread Id failed.\n");
		enqueue(&readyThreadQueue, tempWaitingThread);
	}	
	enqueue(&terminatedThreadQueue, currentThread);
	sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
	raise(SIGNAL_TYPE);
}

static void wrapperFunction(void *(*start_routine)(void *), void *arg)
{
	void* returnValue = NULL;
	sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
	returnValue = start_routine(arg);
	gtthread_exit(returnValue);
}

int gtthread_create(gtthread_t *thread, void *(*start_routine)(void *), void *arg)
{
	threadListNode* newThreadNode = NULL;
	sigprocmask(SIG_BLOCK, &signalMask, NULL);
	if((!start_routine) || (!thread))
	{
		printf("gtthread_create() : FAILED - Argument Check.\n");
		sigprocmask(SIG_UNBLOCK, &signalMask, NULL);		
		return -1;
	}	
	newThreadNode = createThreadNode();
	if(!newThreadNode)
	{
		printf("gtthread_create() : FAILED on node creation\n");
		sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
		return -1;
	}
	newThreadNode->threadContext = (ucontext_t*)malloc(sizeof(ucontext_t));
	if(!(newThreadNode->threadContext))
	{
		free(newThreadNode);
		printf("gtthread_create(): Error while allocating space for new thread node's context.\n");
		sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
		return -1;
	}
	getcontext(newThreadNode->threadContext);
	newThreadNode->threadContext->uc_stack.ss_sp = malloc(STACK_SIZE);
	if(newThreadNode->threadContext->uc_stack.ss_sp == NULL)
	{
		free(newThreadNode->threadContext);
		free(newThreadNode);
		printf("\ngtthread_create() : FAILED on stack memory allocation\n");
		sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
		return -1;
	}
	newThreadNode->threadContext->uc_link = NULL;
	newThreadNode->threadContext->uc_stack.ss_flags = 0;
	newThreadNode->threadContext->uc_stack.ss_size = STACK_SIZE;
	makecontext(newThreadNode->threadContext, (void (*) (void))wrapperFunction, 2, start_routine, arg);
	*thread = newThreadNode->threadId;
	enqueue(&readyThreadQueue, newThreadNode);
	sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
	return 0;
}

static void freeThreadNode(threadListNode* threadNode)
{
	if(!threadNode)
	{
		printf("The thread node to be freed is empty.\n");
		return;
	}
	if(!(threadNode->isMain))
		free(threadNode->threadContext->uc_stack.ss_sp);
	free(threadNode->threadContext);
	free(threadNode);
}

static threadListNode* lookUpThreadNode(threadListPtr threadList, gtthread_t thread)
{
	threadListNode* tempIterator = NULL;
	tempIterator = threadList->next;
	do
	{
		if(tempIterator->threadId == thread)
		{
			return tempIterator;
		}
		tempIterator = tempIterator->next;
	}while(tempIterator != threadList->next);
	return NULL;
}

int  gtthread_join(gtthread_t thread, void **status)
{
	threadListNode* threadToJoin = NULL;
	sigprocmask(SIG_BLOCK, &signalMask, NULL);
	if((currentThread->threadId) == thread)
	{
		printf("gtthread_join() : The thread is trying to join itself.\n");
		sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
		return -1;
	}
	if(readyThreadQueue)
	{
		threadToJoin = lookUpThreadNode(readyThreadQueue,thread);
	}
	if(threadToJoin)
	{
		if(threadToJoin->waitingThreadNode)
		{
			sigprocmask(SIG_UNBLOCK, &signalMask, NULL);			
			return -1;
		}
		threadToJoin->waitingThreadNode = currentThread;
		currentThread->joinStatus = status;
		currentThread->isWaiting = 1;
		enqueue(&waitingThreadQueue, currentThread);
		sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
		raise(SIGNAL_TYPE);
	}	
	else
	{
		if(waitingThreadQueue)
		{
			threadToJoin = lookUpThreadNode(waitingThreadQueue,thread);	
		}	
		
		if(threadToJoin)
		{
			if(threadToJoin->waitingThreadNode)
			{
				sigprocmask(SIG_UNBLOCK, &signalMask, NULL);				
				return -1;
			}
			threadToJoin->waitingThreadNode = currentThread;
			currentThread->joinStatus = status;
			currentThread->isWaiting = 1;
			enqueue(&waitingThreadQueue, currentThread);
			sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
			raise(SIGNAL_TYPE);
		}		
		else
		{
			threadToJoin = dequeueByThreadId(&terminatedThreadQueue, thread);
			if(threadToJoin)
			{
				if(threadToJoin->waitingThreadNode)
				{
					sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
					return -1;
				}
				if(status)
				{
					*status = threadToJoin->returnValue;
				}
				freeThreadNode(threadToJoin);
			}
			else
			{
				printf("Thread to join not in ready, waiting and terminated thread queues.\n");
				sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
				return -1;
			}
		}
	}
	sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
	return 0;
}	

gtthread_t gtthread_self(void)
{
	gtthread_t returnId;
	sigprocmask(SIG_BLOCK, &signalMask, NULL);
	returnId = currentThread->threadId;
	sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
	return returnId;	
}

int gtthread_equal(gtthread_t t1, gtthread_t t2) 
{
    	int returnValue = 0;
   	sigprocmask(SIG_BLOCK, &signalMask, NULL);
   	if(t1 == t2)
		returnValue = 1;
        sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
        return returnValue;
}

int gtthread_yield(void)
{
	raise(SIGNAL_TYPE);
	return 0;
}

int  gtthread_cancel(gtthread_t thread)
{
	threadListNode* nodeToCancel = NULL; 
	threadListNode* tempWaitingThread = NULL;
	sigprocmask(SIG_BLOCK, &signalMask, NULL);
	nodeToCancel = dequeueByThreadId(&readyThreadQueue, thread);
	if(!nodeToCancel)
		nodeToCancel = dequeueByThreadId(&waitingThreadQueue, thread);
	if(!nodeToCancel)
	{
		printf("gtthread_cancel(): Node not found in ready or waiting queue.\n");
		sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
		return -1;
	}
	nodeToCancel->returnValue = NULL;	
	nodeToCancel->hasTerminated = 1;	
	if(nodeToCancel->waitingThreadNode)
	{	
		if((nodeToCancel->waitingThreadNode)->joinStatus)
			*((nodeToCancel->waitingThreadNode)->joinStatus) = nodeToCancel->returnValue;
		(nodeToCancel->waitingThreadNode)->isWaiting = 0;
		tempWaitingThread = dequeueByThreadId(&waitingThreadQueue, (nodeToCancel->waitingThreadNode)->threadId);
		enqueue(&readyThreadQueue, tempWaitingThread);
	}	
	enqueue(&terminatedThreadQueue, nodeToCancel);
	sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
	return 0;	
}
