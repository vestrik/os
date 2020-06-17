/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *  Modified by A.Kostin
 ********************************************************
 * mutex.c
 *
 * Simple multi-threaded example with a mutex lock.
 */
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

void do_one_thing(int *);
void do_another_thing(int *);
void do_wrap_up(int);
int common = 0; /* A shared variable for two threads */
int r1 = 0, r2 = 0, r3 = 0;
pthread_mutex_t mut1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut2 = PTHREAD_MUTEX_INITIALIZER;
sem_t sem1;
sem_t sem2;

int main() {
  pthread_t thread1, thread2;
  int res = sem_init(&sem1, 0, 0);
  int res2 = sem_init(&sem2, 0, 0);

  if (pthread_create(&thread1, NULL, (void *)do_one_thing,
			  (void *)&common) != 0) {
    perror("pthread_create");
    exit(1);
  }    
  

  if (pthread_create(&thread2, NULL, (void *)do_another_thing,
                     (void *)&common) != 0) {
    perror("pthread_create");
    exit(1);
  }

  if (pthread_join(thread1, NULL) != 0) {
    perror("pthread_join");
    exit(1);
  }

  if (pthread_join(thread2, NULL) != 0) {
    perror("pthread_join");
    exit(1);
  }
   sem_destroy(&sem1);
   sem_destroy(&sem2);

 //do_wrap_up(common);

  return 0;
}

void do_one_thing(int *pnum_times) {
  int i, j, x;
  unsigned long k;
  int work;  
  for (i = 0; i < 20; i++) {        
    //pthread_mutex_lock(&mut1);    
    printf("doing one thing\n");
    work = i;
    printf("counter = %d\n", work);
    work++; 
    for (k = 0; k < 500000; k++)
      ;                 
    *pnum_times = work;     
	//pthread_mutex_unlock(&mut1);  
    sem_post(&sem1);  
    sem_wait(&sem2);
     
  }
  sem_post(&sem1); 
  
}

void do_another_thing(int *pnum_times) {
  int i, j, x;
  unsigned long k;
  int work;  
  sem_wait(&sem1); 
  for (i = 0; i < 20; i++) {
    //wait(1);    
   // pthread_mutex_lock(&mut1);     
    printf("doing another thing\n");
    work = i;
    printf("counter = %d\n", work);
    work++; 
    for (k = 0; k < 500000; k++)
      ;                 
    *pnum_times = work;     
    //pthread_mutex_unlock(&mut1);    
    sem_post(&sem2); 
    sem_wait(&sem1);
  }
  sem_post(&sem2); 
}

void do_wrap_up(int counter) {
  int total;
  printf("All done, counter = %d\n", counter);
}
