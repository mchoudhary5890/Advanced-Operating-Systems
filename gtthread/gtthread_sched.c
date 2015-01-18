#define TIMER_TYPE ITIMER_VIRTUAL
#include "shared.h"
void scheduler(int sigNum)
{
	threadListNode* candidate = NULL;
	sigprocmask(SIG_BLOCK, &signalMask, NULL);	
	candidate = dequeue(&readyThreadQueue);
	if(!candidate)
	{
		if(currentThread->isWaiting)
		{		
			printf("[DEBUG]: No Runnable Thread. Exiting..\n");
			exit(1);
		}
		if(!(currentThread->hasTerminated))
		{
			setitimer(TIMER_TYPE, &quantum, NULL);
			sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
			return;
		}
		else
		{
			sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
			exit(0);
		}
	}
	else
	{
		if(currentThread->hasTerminated)
		{	
			currentThread = candidate;
			setitimer(TIMER_TYPE, &quantum, NULL);
			sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
			setcontext(currentThread->threadContext);
		}
		else if(currentThread->isWaiting)
		{
			currentThread = candidate;
			setitimer(TIMER_TYPE, &quantum, NULL);
			sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
			swapcontext(waitingThreadQueue->threadContext, currentThread->threadContext);	
		}
		else
		{	
			enqueue(&readyThreadQueue, currentThread);
			currentThread = candidate;
			setitimer(TIMER_TYPE, &quantum, NULL);
			sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
			swapcontext(readyThreadQueue->threadContext, currentThread->threadContext);
		}
	}
}

