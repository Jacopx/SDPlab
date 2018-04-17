/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *        System and Device Programming - Lab 4.2 == Jacopo Nasi           *
 *       Repository available on https://github.com/Jacopx/SDPlab          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

 #include <stdio.h>
 #include <stdlib.h>
 #include <pthread.h>
 #include <time.h>
 #include <string.h>
 #include <semaphore.h>
 #include <unistd.h>

 int main(int argc, char **argv) {
   srand(time(NULL));
   int v1[atoi(argv[1])];

   for(int i = 0; i < atoi(argv[1]); ++i) {
     v1[i] = rand() % 100;
     printf("%d\n", v1[i]);
   }
   fflush(stdout);

   FILE *fpw; // file pointer
   if ((fpw = fopen ("rand-bin.txt", "wb")) == NULL){ // w for write, b for binary
     fprintf(stderr," error open fv1.b"); return(1);
   }

   fwrite(v1, sizeof(int), atoi(argv[1]), fpw); // write from buffer
   fclose(fpw);
 }
