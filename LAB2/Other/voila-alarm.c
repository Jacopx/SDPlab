/*
voila-alarm.c

Illustrates the use of the system calls
signal, alarm, and pause
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void catcher();
void catcher1();


int i = 1;

//  "interrupt" handler
void catcher() {
  system("date");
  printf("voila'\n");
  if (i == 1)
    signal(SIGALRM,catcher1);
  else
    signal(SIGALRM,catcher);
  i = 3-i;
}


void catcher1() {
  system("date");
  printf("here I'm\n");
  if (i == 1)
    signal(SIGALRM,catcher1);
  else
    signal(SIGALRM,catcher);
  i = 3-i;				// swap 1 and 2
}

int main() {
  int i;

  signal(SIGALRM,catcher);	// declaration for the kernel. It means:
  					// I don't want to die (default action)
  					// when I receive a SIGALRM signal, instead
  					// I will execute function "catcher"

  while(1) {
    alarm(5);			// is not blocking
    printf("waiting begins\n");
    pause();
    printf("end wait\n");
  }
  exit(0);
}
