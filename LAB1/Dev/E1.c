#include <stdlib.h>
#include <stdio.h>
#include <time.h>
//#define VERB

int main(int argc, char **argv) {
  int n = (int) atoi(argv[1]), k = (int) atoi(argv[2]), r, nr;
  char cmd[50];

  sprintf(cmd, "mkdir data");
  system("rm -rf data");
  system(cmd); // Run command in cmd from the same process
  srand(time(NULL));

  for(int i=0; i<n; ++i) {
    sprintf(cmd, "touch data/f%d.txt", i);
    system(cmd); // Run command in cmd from the same process

    nr = rand()%100; // #ofRandomNumber

    #ifdef VERB
        printf("#randNum %d in file: f%d\n", nr, i); //debug purpose
    #endif

    // nr Random number written in the file
    for(int j=0; j<nr; ++j) {
      r = rand()%k;
      sprintf(cmd, "echo '%d' >> data/f%d.txt", r, i);
      system(cmd); // Run command in cmd from the same process

      #ifdef VERB
        printf("rand %d in file: f%d\n", r, i); //debug purpose
      #endif
    }
  }
}
