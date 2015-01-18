#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>


/* Queue Structures */

typedef struct queue_node_s {
  struct queue_node_s *next;
  struct queue_node_s *prev;
  char c;
} queue_node_t;

typedef struct {
  struct queue_node_s *front;
  struct queue_node_s *back;
  pthread_mutex_t lock;
} queue_t;


/* Thread Function Prototypes */
void *producer_routine(void *arg);
void *consumer_routine(void *arg);


/* Global Data */
long g_num_prod; /* number of producer threads */
pthread_mutex_t g_num_prod_lock;


/* Main - entry point */
int main(int argc, char **argv) {
  queue_t queue;
  pthread_t producer_thread, consumer_thread;
  void *thread1_return = NULL;
  void *thread2_return = NULL;
  int result = 0;

  /* Initialization */

  printf("Main thread started with thread id %lu\n", pthread_self());

  memset(&queue, 0, sizeof(queue));
  pthread_mutex_init(&queue.lock, NULL);
  /* <<<<< BUG >>>>>
  A mutex is created by calling the pthread_mutex_init subroutine and the ID of the created mutex is returned to the calling thread through    
  the mutex parameter. A mutex must be created once. 
  In original code, we are calling pthread_mutex_init() only on queue.lock mutex but not on g_num_prod_lock.
  */
  pthread_mutex_init(&g_num_prod_lock, NULL);

  g_num_prod = 1; /* there will be 1 producer thread */

  /* Create producer and consumer threads */

  result = pthread_create(&producer_thread, NULL, producer_routine, &queue);
  if (0 != result) {
    fprintf(stderr, "Failed to create producer thread: %s\n", strerror(result));
    exit(1);
  }

  printf("Producer thread started with thread id %lu\n", producer_thread);

  /* <<<<<BUG>>>>>
  Here, we calling pthread_join after pthread_detach. A detached thread can't be joined. 
  So, it is a bug. If we want to wait for the producer thread to finish, we should call pthread_join() only.
  */
  //result = pthread_detach(producer_thread);
  //if (0 != result)
  //  fprintf(stderr, "Failed to detach producer thread: %s\n", strerror(result));

  result = pthread_create(&consumer_thread, NULL, consumer_routine, &queue);
  if (0 != result) {
    fprintf(stderr, "Failed to create consumer thread: %s\n", strerror(result));
    exit(1);
  }
  printf("Main has created the consumer thread..\n");

  /* Join threads, handle return values where appropriate */

  result = pthread_join(producer_thread, &thread2_return);
  if (0 != result) {
    fprintf(stderr, "Failed to join producer thread: %s\n", strerror(result));
    pthread_exit(NULL);
  }

  result = pthread_join(consumer_thread, &thread1_return);
  if (0 != result) {
    fprintf(stderr, "Failed to join consumer thread: %s\n", strerror(result));
    pthread_exit(NULL);
  }
  /* <<<<< BUG >>>>>
  1. Wrong type casting. The value of count is returned as (void *) and saved at the address of thread1_return. So, to get the value for    
     printing we just need tp type cast with (long).
  2. In the original code, we are trying to free the void pointer. But, free is called when we have allocated some memory from the heap which 
     is done using malloc/ calloc.  Here, we are not allocating any dynamic memory and thus, there is no need to call free. We are just 
     storing the value of count at the address of the void pointer.     
     Alternate implementation: Instead of returning value, we can allocate some memory in heap and then, return a reference to that memory  
     which would hold the value of count. In that case, we would be required to free the memory allocated dynamically.
  3. It is not a bug but i thought of implementing this. In original code, we were handling the return value from one consumer thread  only. I 
     have made the producer thread to wait for the consumer thread and get the value of count. Producer thread would return this value to 
     main. Finally, main can print the count value for both consumer threads as well as the total number of printed characters. 

  */
  printf("\nConsumer from main printed %lu characters.\n", (long)thread1_return); // Changed here ... free is not required
  printf("\nConsumer from producer printed %lu characters.\n", (long)thread2_return);
  printf("\nTotal printed %lu characters.\n", (long)thread2_return+(long)thread1_return);
  pthread_mutex_destroy(&queue.lock);
  pthread_mutex_destroy(&g_num_prod_lock);
  return 0;
}


/* Function Definitions */

/* producer_routine - thread that adds the letters 'a'-'z' to the queue */

/* <<<<< NOTE >>>>> 
I have removed detach funtion from here because I want to make the producer to wait for consumer threat created by it and get the return value which would be passed to the main function.
*/

void *producer_routine(void *arg) {
  queue_t *queue_p = arg;
  queue_node_t *new_node_p = NULL;
  pthread_t consumer_thread;
  int result = 0;
  void *thread_return = NULL;
  char c;

  result = pthread_create(&consumer_thread, NULL, consumer_routine, queue_p);
  if (0 != result) {
    fprintf(stderr, "Failed to create consumer thread: %s\n", strerror(result));
    exit(1);
  }
  printf("Producer has created the consumer thread..\n");


  for (c = 'a'; c <= 'z'; ++c) {

    /* Create a new node with the prev letter */
    new_node_p = malloc(sizeof(queue_node_t));
    new_node_p->c = c;
    new_node_p->next = NULL;

    /* Add the node to the queue */
    pthread_mutex_lock(&queue_p->lock);
    printf("\nI am Producer (thread id) %lu\t", pthread_self());
    printf("Adding to the Queue: %c i.e. %c\n", new_node_p->c, c);
    if (queue_p->back == NULL) {
      assert(queue_p->front == NULL);
      new_node_p->prev = NULL;

      queue_p->front = new_node_p;
      queue_p->back = new_node_p;
    }
    else {
      assert(queue_p->front != NULL);
      new_node_p->prev = queue_p->back;
      queue_p->back->next = new_node_p;
      queue_p->back = new_node_p;
    }
    pthread_mutex_unlock(&queue_p->lock);

    sched_yield();
  }
  
  /* Decrement the number of producer threads running, then return */
  /* <<<<< BUG >>>>>
  the prefix decrement operation on g_num_prod is not atomic. There may be some scenario when it is preempted in mid or running in parallel(in 
  case of multi-processing) and its not updated value is read by the consumer thread. This may result in inconsistency. 
  So, the operation should be performed after acquiring the lock g_num_prod_lock.
  */
  pthread_mutex_lock(&g_num_prod_lock);
  --g_num_prod;
  pthread_mutex_unlock(&g_num_prod_lock);
  result = pthread_join(consumer_thread, &thread_return);
  if (0 != result)
    fprintf(stderr, "Failed to attach consumer thread: %s\n", strerror(result));
  return thread_return;
}


/* consumer_routine - thread that prints characters off the queue */
void *consumer_routine(void *arg) {
  queue_t *queue_p = arg;
  queue_node_t *prev_node_p = NULL;
  long count = 0; /* number of nodes this thread printed */

  printf("Consumer thread started with thread id %lu\n", pthread_self());

  /* terminate the loop only when there are no more items in the queue
   * AND the producer threads are all done */
/* <<<<< BUG >>>>>
In the original code, there is problem with locking and unlocking because of which the output is inconsistent. 
In second onwards iterations of while loop, we are unlocking the mutex without locking it.
To, fix this, some modifications have been done in the code.

pthread_mutex_lock(&queue_p->lock); has been moved inside the loop because we are making a check on queue status inside the if condition after aquiring lock. So, even if we don't get lock for the condition check in while, we are fine.
 
pthread_mutex_lock(&g_num_prod_lock); has been added at the end of if block so that lock is there for the check of g_num_prod in the next iteration of while loop.
*/

  pthread_mutex_lock(&g_num_prod_lock);
  while(queue_p->front != NULL || g_num_prod > 0) {
    pthread_mutex_unlock(&g_num_prod_lock);
    pthread_mutex_lock(&queue_p->lock);
    if (queue_p->front != NULL) {
      /* Remove the prev item from the queue */
      prev_node_p = queue_p->front;
      if (queue_p->front->next == NULL)
        queue_p->back = NULL;
      else
        queue_p->front->next->prev = NULL;
      queue_p->front = queue_p->front->next; 
      /* Print the character, and increment the character count */
      printf("\nI am Consumer (thread id) %lu\t Extracting: %c", pthread_self(),prev_node_p->c);
      pthread_mutex_unlock(&queue_p->lock);
      free(prev_node_p);
      ++count;
      pthread_mutex_lock(&g_num_prod_lock);
    }
    else { /* Queue is empty, so let some other thread run */
      pthread_mutex_unlock(&queue_p->lock);
      sched_yield();
    }
  }
  pthread_mutex_unlock(&g_num_prod_lock);
  pthread_mutex_unlock(&queue_p->lock);

  return (void*)count; 
}
