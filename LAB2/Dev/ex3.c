#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>

#define ODD(x) ((x) % (2) != 0 ? (x) : (x + 1))
#define EVEN(x) ((x) % (2) == 0 ? (x) : (x + 1))

void *print(void *args);

int g;
sem_t *s;
int thread_ended;
pthread_mutex_t te_me = PTHREAD_MUTEX_INITIALIZER;

struct blk {
  char file[20];
  int s;
};

int main(int argc, char **argv) {
  pthread_t tidp1, tidp2;
  int n1 = atoi(argv[1]), n2 = atoi(argv[2]), i = 0, totReq = 0;

  int *v1 = (int *) malloc(n1 * sizeof(int));
  int *v2 = (int *) malloc(n2 * sizeof(int));
  struct blk *myBlk1 = (struct blk *) malloc(sizeof(struct blk));
  struct blk *myBlk2 = (struct blk *) malloc(sizeof(struct blk));

  srand(time(NULL));

  for(i = 0; i < n1; ++i) {
    v1[i] = ODD(rand() % 91 + 10);

    printf("%d (%d) ", v1[i], 3*v1[i]);
  }
  printf("\n");
  for(i = 0; i < n2; ++i) {
    v2[i] = EVEN(rand() % 81 + 21);
    printf("%d (%d) ", v2[i], 3*v2[i]);
  }
  printf("\n");
  fflush(stdout);

  FILE *fpw; // file pointer
  if ((fpw = fopen ("fv1.b", "wb")) == NULL){ // w for write, b for binary
    fprintf(stderr," error open fv1.b"); return(1);
  }

  fwrite(v1, sizeof(int), n1, fpw); // write from buffer
	fclose(fpw);

  if ((fpw = fopen ("fv2.b", "wb")) == NULL){ // w for write, b for binary
    fprintf(stderr," error open fv2.b"); return(1);
  }

  fwrite(v2, sizeof(int), n2, fpw); // write from buffer
  fclose(fpw);

  strcpy(myBlk1->file, "fv1.b");
  strcpy(myBlk2->file, "fv2.b");

	s = malloc(3 * sizeof(sem_t));
	for (i = 0; i < 3; ++i) {
		sem_init(&s[i], 0, 0);
	}

  myBlk1->s=1;
  myBlk2->s=2;

  pthread_create(&tidp1, NULL, print, (void *) myBlk1);
  pthread_create(&tidp2, NULL, print, (void *) myBlk2);

  while(1) {
    sem_post(&s[0]);
    sem_wait(&s[1]);
    if (thread_ended == 2)
      break;
    g *= 3;
    sem_post(&s[2]);
    sem_wait(&s[1]);
    ++totReq;
  }

  pthread_join(tidp1, NULL);
  pthread_join(tidp2, NULL);

  printf("\nServer total requests: %d\n", totReq);

  // free
  for (i = 0; i < 3; ++i)
    sem_destroy(&s[i]);
  free(s);
  pthread_mutex_destroy(&te_me);

  free(myBlk1);
  free(myBlk2);

  return 0;
}

void *print(void *args) {
  struct blk *myBlk = (struct blk *) args;
  FILE *fp;
  fp = fopen(myBlk->file, "rb"); // r for read, b for binary

  int value;
	while (fread(&value, sizeof(int), 1, fp)) {
    sem_wait(&s[0]);
    g = value;
    sem_post(&s[1]);
    sem_wait(&s[2]);
    printf("%d == %ld\n", g, pthread_self());
    sem_post(&s[1]);

  }

  pthread_mutex_lock(&te_me);
  thread_ended++;
  if (thread_ended == 2)
    sem_post(&s[1]);
  pthread_mutex_unlock(&te_me);

  pthread_exit (0);
}
