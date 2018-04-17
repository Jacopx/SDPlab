/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *        System and Device Programming - Lab 4.1 == Jacopo Nasi           *
 *       Repository available on https://github.com/Jacopx/SDPlab          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <time.h>
 #include <pthread.h>
 #include <sys/mman.h>
 #include <fcntl.h>
 #include <sys/stat.h>

 #define N 1000
 int size;

 void *quicksort(void *);
 void swap(int *, int, int);

 struct blk {
   int *v;
   int left;
   int right;
 };

 int main(int argc, char **argv) {
   int i = 0, n = 0, f_size, p_size;
   pthread_t tid;

   // Size Value by User
   size = atoi(argv[2]);

   int fd;
   fd = open(argv[1],O_RDWR);

   struct stat st;
   fstat(fd,&st);
   f_size=st.st_size;
   p_size=getpagesize();
   n = f_size / sizeof(int);
   f_size=f_size + p_size -(f_size%p_size);

   struct blk *myBlk = (struct blk *) malloc(sizeof(struct blk));
   myBlk->v = (int *) mmap((void *) 0,f_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
   myBlk->left=0;
   myBlk->right=n-1;

   printf("Not ordered:\n");

   for(i=0; i<n; ++i) {
     printf("%d ", myBlk->v[i]);
   }

   printf("\n");

   pthread_create(&tid, NULL, quicksort, (void *) myBlk);
   pthread_join(tid, NULL);

   printf("Ordered:\n");
   for(i=0; i<n; ++i) {
     printf("%d ", myBlk->v[i]);
   }

   return 0;
 }

void *quicksort(void *args) {
   int i, j, x;
   struct blk *myBlk = (struct blk *) args;
   pthread_t tid1, tid2;

   if (myBlk->left >= myBlk->right) return NULL;

   x = myBlk->v[myBlk->left];
   i = myBlk->left - 1;
   j = myBlk->right + 1;

   while (i < j) {
     while (myBlk->v[--j] > x);
     while (myBlk->v[++i] < x);
     if (i < j) {
       swap(myBlk->v,i,j);
     }
   }

   // for(i=0; i<n; ++i) {
   //   printf("%d ", myBlk->v[i]);
   // }
   // printf("\n");

   struct blk *myBlk1 = (struct blk*) malloc(sizeof(struct blk));
   struct blk *myBlk2 = (struct blk*) malloc(sizeof(struct blk));

   myBlk1->v = myBlk->v;
   myBlk2->v = myBlk->v;

   myBlk1->left = myBlk->left;
   myBlk1->right = j;

   myBlk2->left = j + 1;
   myBlk2->right = myBlk->right;

   if (myBlk->right - myBlk->left < size) {
     quicksort((void *) myBlk1);
     quicksort((void *) myBlk2);
     return NULL;
   }

   // Recursive call
   pthread_create(&tid1, NULL, quicksort, (void *) myBlk1);
   pthread_create(&tid2, NULL, quicksort, (void *) myBlk2);
   pthread_join(tid1, NULL);
   pthread_join(tid2, NULL);

   return NULL;
}

void swap(int *v, int i, int j){
   int tmp;
   tmp = v[i];
   v[i] = v[j];
   v[j] = tmp;
}
