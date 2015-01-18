#ifndef __STRUCTURES_H
#define __STRUCTURES_H

#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned long gtthread_t;
typedef unsigned long gtthread_mutex_t;
	
typedef struct threadNode{
	int isMain;
        int isWaiting; 
	int hasTerminated;
	void* returnValue;
	void** joinStatus;
	gtthread_t threadId;	
	struct threadNode* next;
	ucontext_t* threadContext;
	struct threadNode* waitingThreadNode;
} threadListNode;

typedef threadListNode* threadListPtr;

typedef struct mutexWaitingNode{
	threadListNode* threadPtr;
	struct mutexWaitingNode* next;
} mutexWaitingListNode;

typedef mutexWaitingListNode* mutexWaitingListPtr;

typedef struct mutexNode{
	gtthread_mutex_t mutexId;
	int isLocked;
	threadListNode* currentOwnerThread;
	mutexWaitingListNode* mutexWaitingList;
	struct mutexNode* next;
} mutexListNode;

typedef mutexListNode* mutexListPtr;

threadListNode* dequeue(threadListPtr*);
void enqueue(threadListPtr*, threadListNode*);
threadListNode* dequeueByThreadId(threadListPtr*, gtthread_t);
void printQueue(threadListPtr);
void scheduler(int);

#endif
