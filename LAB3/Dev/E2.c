#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct {
  pthread_mutex_t lock;
  pthread_cond_t cond;
  int nR;
} Cond;

// Global variables
Cond *cond;
pthread_mutex_t meW;

long long current_timestamp () {
  struct timeval te;
  gettimeofday(&te, NULL);
  long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
  return milliseconds;
}

void* reader (void *arg) {
  long long int t;

  usleep((rand() % 501) * 1000);
  t = current_timestamp();
  printf("Thread %ld trying to read at time %lld\n", (long) pthread_self(), t);

  pthread_mutex_lock(&cond->lock);
  cond->nR++;
  pthread_mutex_unlock(&cond->lock);

  t = current_timestamp();
  printf("Thread %ld reading at time %lld\n", (long) pthread_self(), t);
  usleep(500 * 1000);
  printf("Thread %ld end reading\n", (long) pthread_self());

  pthread_mutex_lock(&cond->lock);
  cond->nR--;
  if (cond->nR == 0)
    pthread_cond_signal(&cond->cond);
  pthread_mutex_unlock(&cond->lock);

  pthread_exit (0);
}

void* writer (void *arg) {
  long long int t;

  usleep((rand() % 501) * 1000);
  t = current_timestamp();
  printf("Thread %ld trying to write at time %lld\n", (long) pthread_self(), t);

  pthread_mutex_lock(&meW);
  pthread_mutex_lock(&cond->lock);
  while (cond->nR != 0)
    pthread_cond_wait(&cond->cond, &cond->lock);

  t = current_timestamp();
  printf("Thread %ld writing at time %lld\n", (long) pthread_self(), t);
  usleep(500 * 1000);
  printf("Thread %ld end writing\n", (long) pthread_self());

  pthread_mutex_unlock(&cond->lock);
  pthread_mutex_unlock(&meW);

  pthread_exit (0);
}


int main (int argc, char **argv) {
  pthread_t *tids_reader, *tids_writer;
  int i, n;
  srand(time(NULL));

  if (argc != 2) {
    printf("Specify a number as argument\n");
    exit (0);
  }

  cond = (Cond *) malloc(sizeof(Cond));
  pthread_mutex_init(&cond->lock, NULL);
  pthread_cond_init(&cond->cond, NULL);
  cond->nR = 0;
  pthread_mutex_init(&meW, NULL);

  n = atoi(argv[1]);
  tids_reader = (pthread_t *) malloc(n * sizeof(pthread_t));
  tids_writer = (pthread_t *) malloc(n * sizeof(pthread_t));

  for (i = 0; i < n; ++i) {
    pthread_create(&tids_reader[i], NULL, reader, NULL);
    pthread_create(&tids_writer[i], NULL, writer, NULL);
  }

  for (i = 0; i < n; ++i) {
    pthread_join(tids_reader[i], NULL);
    pthread_join(tids_writer[i], NULL);
  }

  pthread_mutex_destroy(&meW);

  free(cond);
  free(tids_reader);
  free(tids_writer);

  return 0;
}
