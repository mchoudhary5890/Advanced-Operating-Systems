#include <omp.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
int max;
void barrier(int * count,int * sense,int* local)
{

*local=!(*local);
#pragma omp critical
{
*count-=1;
}
if(*count==0)
{
*count=max;
*sense=*local;
}
else
{
while(*sense!=*local)
{}
}
}

int main(int argc, char **argv)
{
  if(argc!=2)
  {
  	printf("./central <num  of threads>\n"); 
        exit(0);
  }
  struct timeval start, end;
  int count, sense=1;
  count=atoi(argv[1]);
  max=count;
  omp_set_num_threads(count);
  gettimeofday(&start, NULL);
#pragma omp parallel shared(count,sense)
  {
  int  thread_num = omp_get_thread_num();
    printf("thread %d: hello\n", thread_num);
    int local=1;
  int itr=0;int itr1=0;
   for(itr=0;itr<100;itr++)
   {
   for(itr1=0;itr1<1000;itr1++);
  printf("before barrier %d in thread %d\n",itr,thread_num);
   barrier(&count,&sense,&local);
  printf("after barrier %d in thread %d\n",itr,thread_num);
   }


  }
gettimeofday(&end, NULL);
printf("%REACHED END TIME TAKEN IS    %ld\n", ((end.tv_sec * 1000000 + end.tv_usec)
		  - (start.tv_sec * 1000000 + start.tv_usec)));

  return 0;
}

