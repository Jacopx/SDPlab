#include <stdio.h>
#include "pthread.h"
#include "semaphore.h"

sem_t *sem;

static void *
B (void *arg)
{

  pthread_detach (pthread_self ());
  sleep(5);
  printf ( "B\n");
  sem_post(sem);
  printf ( "D\n");
  return  0;
}  

int
main (void)
{
  pthread_t th_a;
  void *retval;

  sem = (sem_t *) malloc(sizeof(sem_t));
  sem_init (sem, 0, 0);
  
   printf("A\n");
  /* Create the threads */
  pthread_create (&th_a, NULL, B, 0);
  
  sem_wait(sem);
  printf("C\n");
  pthread_exit(0);
}
