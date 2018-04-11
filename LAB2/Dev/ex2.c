#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *doubleLeaf(void *myInfo);

struct info {
    long int n[1000];
    int lv;
};

int t;
pthread_t tidp;

int main (int argc, char *argv[]){
    t = atoi(argv[1]); // Number of level

    struct info *myInfo1 = (struct info*) malloc(sizeof(struct info));
    struct info *myInfo2 = (struct info*) malloc(sizeof(struct info));

    myInfo1->lv = 0;
    myInfo1->n[myInfo1->lv] = (int) pthread_self();
    myInfo2->lv = 0;
    myInfo2->n[myInfo2->lv] = (int) pthread_self();


    pthread_create(&tidp, NULL, doubleLeaf, (void *) myInfo1);
    sleep(1);
    pthread_create(&tidp, NULL, doubleLeaf, (void *) myInfo2);

    pthread_exit (NULL);
}

void *doubleLeaf(void *args) {
    //printf("Inside thread...");
    struct info *myInfo = (struct info*) args;

    struct info *myInfo1 = (struct info*) malloc(sizeof(struct info));
    struct info *myInfo2 = (struct info*) malloc(sizeof(struct info));

    for(int i=0;i<=myInfo->lv;++i) {
        myInfo1->n[i]=myInfo->n[i];
        myInfo2->n[i]=myInfo->n[i];
    }

    myInfo1->lv = myInfo->lv + 1;
    myInfo1->n[myInfo1->lv] = (int) pthread_self();
    myInfo2->lv = myInfo->lv + 1;
    myInfo2->n[myInfo2->lv] = (int) pthread_self();

    if(myInfo->lv + 1 < t) {

        pthread_create(&tidp, NULL, doubleLeaf, (void *) myInfo1);
        sleep(1);
        pthread_create(&tidp, NULL, doubleLeaf, (void *) myInfo2);

    } else {
        fprintf (stdout, "FIN: ");
        for(int i = 0; i<=myInfo1->lv; ++i) {
            fprintf(stdout, "%lu ", myInfo1->n[i]);
        }
        printf("\n");
    }

    pthread_exit(NULL);
}
