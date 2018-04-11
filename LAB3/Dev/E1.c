// Producer and Consumer example u07s05 @ Pag. 4

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/time.h>

#define SIZE 16

// Prototypes
long long current_timestamp();
void enqueue(long long, int);
void dequeue(long long *, int);
void *consumer(void *);
void *producer(void *);
void init();

// Global Variables
long normal[SIZE], urgent[SIZE];
int inN, outN, nN, inU, outU, nU, p = 0;
sem_t *s; // full, empty;
int thread_ended;
pthread_mutex_t te_me = PTHREAD_MUTEX_INITIALIZER;

// Main thread
int main(int argc, char **argv) {
  pthread_t prod, cons;

  p = atoi(argv[1]);

  init();

  s = (sem_t *) malloc(2 * sizeof(sem_t));

  // FULL Semaphore
  sem_init(&s[0], 0, 0);

  // EMPTY Semaphore
  sem_init(&s[1], 0, SIZE);

  pthread_create(&prod, NULL, producer, NULL);
  pthread_create(&cons, NULL, consumer, NULL);

  pthread_join(prod, NULL);
  pthread_join(cons, NULL);

  return 0;
}

void *producer(void *args) {
  int i = 0, type = -1;
  long long now;
  srand(time(NULL));

  while(i<100) {
    usleep((rand() % 10) + 1);
    now = current_timestamp(); // Produce

    if((rand() % 100) + 1 < p) {
      type = 0;
    } else {
      type = 1;
    }

    sem_wait(&s[1]); // Wait (empty)
    enqueue(now, type); // Enqueue

    printf("[PROD] TS: %lld [%d]\n", now, type);

    sem_post(&s[0]); // Signal (full)

    ++i;
  }

  pthread_exit(0);
}

void *consumer(void *args) {
  int i = 0, type = -1;
  long long getted;
  while(i<100) {
    usleep(10);

    sem_wait(&s[0]); // Wait (full)

    if(nU > 0) {
      type = 1;
    } else {
      type = 0;
    }
    dequeue(&getted, type); // Dequeue
    sem_post(&s[1]); // Signal (empty)

    printf("[CONS] TS: %lld [%d]\n", getted, type); // Consume

    ++i;
  }

  pthread_exit(0);
}

// Initializing queue
void init() {
  inN = 0; outN = 0; nN = 0; inU = 0; outU = 0; nU = 0;
}

// Professor function to initializing queue indices
long long current_timestamp() {
  struct timeval te;
  gettimeofday(&te, NULL); // get current time
  long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
  return milliseconds;
}

//@TODO Mutex
void enqueue (long long val, int type) {
  if(type) { // Urgent Queue
    urgent[inU] = val;
    inU=(inU+1)%SIZE;
    ++nU;
  } else { // Normal Queue
    normal[inN] = val;
    inN=(inN+1)%SIZE;
    ++nN;
  }
  return;
}

//@TODO Mutex
void dequeue(long long *val, int type) {
  if(type) { // Urgent Queue
    *val=urgent[outU];
    outU=(outU+1)%SIZE;
    --nU;
  } else { // Normal Queue
    *val=normal[outN];
    outN=(outN+1)%SIZE;
    --nN;
  }
  return;
}
