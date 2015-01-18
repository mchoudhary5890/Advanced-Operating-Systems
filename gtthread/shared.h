#ifndef __SHARED_H
#define __SHARED_H

#include <signal.h>
#include "structures.h"
#include <sys/time.h>
#include <string.h>
#define SIGNAL_TYPE SIGVTALRM

extern struct itimerval quantum;
extern struct sigaction signalToScheduler;
extern sigset_t signalMask; 
extern threadListNode* currentThread;
extern threadListPtr readyThreadQueue;
extern threadListPtr terminatedThreadQueue;
extern threadListPtr waitingThreadQueue; 


#endif /* __SHARED_H */
