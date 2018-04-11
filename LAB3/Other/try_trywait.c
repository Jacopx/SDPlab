#include <stdio.h>
#include <errno.h>
#include <semaphore.h>
main() {
  sem_t my_semaphore;
  int value;
  int rc;

  sem_init(&my_semaphore, 0, 1);
  sem_getvalue(&my_semaphore, &value);
  printf("The initial value of the semaphore is %d\n", value);
  sem_wait(&my_semaphore);
  sem_getvalue(&my_semaphore, &value);
  printf("The value of the semaphore after the wait is %d\n", value);
  rc = sem_trywait(&my_semaphore);
  if ((rc == -1) && (errno == EAGAIN)) {
   printf("sem_trywait did not decrement the semaphore\n");
  }

}
