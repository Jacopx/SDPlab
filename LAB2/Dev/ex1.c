#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#define MAXCHAR 50

void catcher();
void catcherS();

int flag = 0;
int go = 1;

int main(int argc, char **argv) {
  char line[MAXCHAR];
  int i=1;

  if(fork()) { // Parent

    signal(SIGUSR1,catcher);

    // Infinite reading cycle
    while(go) {
      FILE *fp = fopen(argv[1], "r");
      while (fgets(line, 50, fp) != NULL) {
        if(flag == 0) {
          printf("#%d - %s", i, line);
        }
        ++i;
        sleep(1);
      }
      i=0;
      fclose(fp);
    }

  } else { // Child

    if(fork()) { // Parent
      signal(SIGUSR2, catcherS);
      while(go) {
        srand(time(NULL));
        sleep(rand() % 10 + 1); // Random number
        kill(getppid(), SIGUSR1); // Send signal to parent
      }
    } else { // Child
      sleep(60);
      kill(getppid(), SIGUSR2); // Send signal to parent
    }
  }

  return 0;
}

void catcher(int SIG) {
  if(SIG == SIGUSR1) {
    flag = 1 - flag;
  } else if(SIG == SIGUSR2) {
    go = 0;
  }
}

void catcherS() {
  go = 0;
  kill(getppid(), SIGUSR2);
}
