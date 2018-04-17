/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *        System and Device Programming - Lab 4.2 == Jacopo Nasi           *
 *       Repository available on https://github.com/Jacopx/SDPlab          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

int activeThreads = 0;
float *v1, *v2, **mat, *v, result = 0, k = 0;

pthread_mutex_t active;

void mult(float *v1, float *v2, float **mat, int k);
void *thMult(void *args);

int main(int argc, char **argv) {
  int i = 0, j = 0;
  pthread_t *tids;

  k = atoi(argv[1]);
  activeThreads = k;

  // Alloc data
  v = malloc(k * sizeof(float));
  v1 = malloc(k * sizeof(float));
  v2 = malloc(k * sizeof(float));
  mat = (float **)malloc(k * sizeof(float*));
  for(i = 0; i < k; ++i)
    mat[i] = (float *)malloc(k * sizeof(float));

  // Alloc thread id vector
  tids = malloc(k * sizeof(pthread_t));

  // Adding random values
  srand(time(NULL));

  for(i = 0; i < k; ++i) {
    v[i] = 0;
    v1[i] = (((float) (rand() % 100) + 1) / 100) - 0.5;
    v2[i] = (((float) (rand() % 100) + 1) / 100) - 0.5;
  }

  for(i = 0; i<k; ++i) {
    for(j = 0; j<k; ++j) {
      mat[i][j] = (((float) (rand() % 100) + 1) / 100) - 0.5;
    }
  }

  mult(v1, v2, mat, k);

  // Creating Threads
  for(i = 0; i<k; ++i) {
    pthread_create(&tids[i], NULL, thMult, (void *) i);
  }

  // Waiting Threads
  for(i = 0; i<k; ++i) {
    pthread_join(tids[i], NULL);
  }

  return 0;
}

void *thMult(void *args) {
  int row = (int) args;
  int i = 0;

  // First multiplication
  for(i = 0; i < k; ++i) {
    v[row] += v2[i] * mat[row][i];
  }

  if(activeThreads == 1) {
    // Second multiplication
    for(i = 0; i < k; ++i) {
      result += v[i] * v1[i];
    }
    printf("Result THREAD: %f\n", result);
  }

  pthread_mutex_lock(&active);
  --activeThreads;
  pthread_mutex_unlock(&active);

  pthread_exit(0);
}

void mult(float *v1, float *v2, float **mat, int k) {
  int j = 0, i = 0;
  float *v = malloc(k * sizeof(float)), result = 0;

  // Clean vector
  for(i = 0; i < k; ++i) {
    v[i] = 0;
  }

  // First multiplication
  for(i = 0; i < k; ++i) {
    for(j = 0; j < k; ++j) {
      v[i] += v2[j] * mat[i][j];
    }
  }

  // Second multiplication
  for(i = 0; i < k; ++i) {
    result += v[i]*v1[i];
  }

  printf("Result NO THREAD: %f\n", result);
}
