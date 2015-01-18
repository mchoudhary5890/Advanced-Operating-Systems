************************************************************
Author : Manish Choudhary
GTID   : 902982487
Date   : Feb 8, 2014
************************************************************

File Details:
	1) Source Code
		a) dining.c - Implementation of dining philosopher problem
		b) gtthread.c -Core thread library functions 
		c) gtthread_mutex.c - Synchronization functions
		d) gtthread_sched.c - Scheduler
	2) Header Files
		a) gtthread.h - Main header file with declarations library functions 
		b) structures.h - All the structures used in the library
		c) shared.h -  The variables shared among the functions across the files
	3) Makefile
		Generates the gtthread.a library and the "dining" binary corresponding to dining.c
________________________________________________________________________________________________

Platform Details:
	 Ubuntu 12.04 (64-bit x86) has been used for the development of this user level thread library

________________________________________________________________________________________________

Preemptive Scheduler Implementation:
	* setitimer is used to trigger an alarm at the interval provided as input to gtthread_init()
	* On timer expiration, SIGVTALRM signal is generated
	* sigaction is used to set scheduler as the handler function for the SIGVTALRM signal
	* The waiting thread is moved to the waitingThreadQueue
	* Terminated thread is moved to the terminatedThreadQueue
	* Scheduler dequeues the thread from the readyThreadQueue
		- If candidate ready thread is found
			* If the current thread(at the time of signal) was terminated, setcontext is used
			* if the current thread is in waiting/ready state, swapcontext is used
		- If the candidate ready thread is not found
			* If the current thread has not terminated, it is allowed to continue
			* Else, there is no runnable thread 
________________________________________________________________________________________________

Steps to Execute:
	* Go the the folder containing the files on the terminal
	* use "make" command to generate "gtthread.a" and "dining"
	* run "./dining" command to execute the binary

________________________________________________________________________________________________

Deadlock Prevention in Dining Philosopher:
	* To prevent deadlock following solution has been implemented
		- Each chopstick has a mutex to control the access to the chopstick
		- Philosophers are considered to be numbered 0 to 4(total 5)
		- Even numbered philosophers pick the right chopstick and then the left chopstick. 
		  Chopsticks are released in the reverse order 
		- Odd numbered philosophers pick the left chopstick first and then the right chopstick.
		  Chopsticks are released in the reverse order
		- This ensures that the two adjacent philosophers contend for the same chopstick.
		  In this way, there would be only one winner between each two.
		  There would never be a case when every philosopher is holding one chopstick and waiting for other. 

_________________________________________________________________________________________________

Thoughts:
	* I really enjoyed working on this project and learnt alot during the development.
	* I have used some ideas that I found searching on web.
		- http://codingfreak.blogspot.com/2009/10/signals-in-linux-blocking-signals.html
	          // To learn how to use sigprocmask() to block/unblock the signals
		- http://frankdrews.com/public_filetree/cs442_542_WQ06/Assignments/Assignment5/assignment5.pdf
		  // Idea of wrapper function to deal with the threads exiting without making a explicit call.	
		- http://pic.dhe.ibm.com/infocenter/zvm/v6r2/index.jsp?topic=%2Fcom.ibm.zvm.v620.edclv%2Fptjoin.htm
                  // join function
	  These are only some that I feel worth mentioning, especially the idea of using wrapper function which is not my own.

_____________________________________END__________________________________________________________




 
