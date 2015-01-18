#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gtthread.h"
#define PHIL_COUNT 5

gtthread_mutex_t chopsticks[PHIL_COUNT];

void* philosopher(void* philIdPtr)
{
	int philId = *(int *)philIdPtr;
	int timer = 0;
	while(1)
	{
		printf("Philospher %d is thinking.\n", philId);
		timer = rand() % 10000 + 1;
		while(timer > 0)
			timer--;	
		if(philId % 2 == 0)
		{
			printf("Philosopher %d is trying to pick the right chopstick.\n", philId);
			gtthread_mutex_lock(&chopsticks[(philId+1) % PHIL_COUNT]);
			printf("Philosopher %d is trying to pick the left chopstick.\n", philId);
			gtthread_mutex_lock(&chopsticks[philId]);
		}
		else
		{
			printf("Philosopher %d is trying to pick the left chopstick.\n", philId);
			gtthread_mutex_lock(&chopsticks[philId]);
			printf("Philosopher %d is trying to pick the right chopstick.\n", philId);
			gtthread_mutex_lock(&chopsticks[(philId+1) % PHIL_COUNT]);
			
		}
		printf("Philosopher %d is eating.\n", philId);
		timer = rand() % 5000 + 1;
		while(timer > 0)
			timer--;		
		if(philId % 2 == 0)
		{
			printf("Philosopher %d is releasing the left chopstick.\n", philId);
			gtthread_mutex_unlock(&chopsticks[philId]);
			printf("Philosopher %d is releasing the right chopstick.\n", philId);
			gtthread_mutex_unlock(&chopsticks[(philId+1) % PHIL_COUNT]);
		}
		else
		{
			printf("Philosopher %d is releasing the right chopstick.\n", philId);
			gtthread_mutex_unlock(&chopsticks[(philId+1) % PHIL_COUNT]);
			printf("Philosopher %d is releasing the left chopstick.\n", philId);
			gtthread_mutex_unlock(&chopsticks[philId]);
		}
	}
	gtthread_exit(NULL);
	return NULL;		
}

int main()
{
	int iterator=0;
	int philId[PHIL_COUNT];
	pthread_t philThread[PHIL_COUNT];
	srand ( time(NULL) );
	gtthread_init(10);
	for(iterator=0; iterator<PHIL_COUNT; iterator++)
		gtthread_mutex_init(&chopsticks[iterator]);
	for(iterator=0; iterator<PHIL_COUNT; iterator++)
	{
		philId[iterator] = iterator;
		gtthread_create(&philThread[iterator], philosopher,(void*)&philId[iterator]);
	}	
	for(iterator=0; iterator<PHIL_COUNT; iterator++)
		gtthread_join(philThread[iterator], NULL);
	return 0;
}
		
