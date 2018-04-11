#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
//#define VERB

int main(int argc, char **argv) {
    int C = (int) atoi(argv[1]), active = 0, i = 0, j = 0;
    char *dir = argv[2], cmd[500], fileName[50], path[50], files[10000] = {""};
    pid_t PID;

    sprintf(cmd, "ls %s > list.txt", dir);
    system(cmd); // Run command in cmd from the same process

    FILE *fp = fopen("list.txt", "r");
    while (fgets(fileName, 50, fp) != NULL) ++i;
    fclose(fp);

    fp = fopen("list.txt", "r");
    while (fgets(fileName, 50, fp) != NULL) {

      sprintf(path, "%s/%s", dir, strtok(fileName, "\n")); // Need to remove /n getted by fgets()
      strcat(files, path);

      if(j<=(i-1)) {
        strcat(files, " ");
      }

      #ifdef VERB
      printf("Active session: %d\n", active);
      #endif

      // Waiting for the max number of child
      if(active >= C) {
          #ifdef VERB
          printf("Waiting...\n");
          #endif

          wait(NULL);
          --active;

          #ifdef VERB
          printf("Resume!\n");
          #endif
      }

      ++active;
      if(!fork()) { // Child
          #ifdef VERB
          sleep(5);
          #endif

          execv("/usr/bin/sort", (char *[]){ "/usr/bin/sort", "-n", "-o", path, path, (char*)0, NULL });
        }
        ++j;
      }

      // Waiting all sons
      while(active>0) {
        wait(NULL);
        --active;
      }

      // Final sort
      fclose(fp);
      sprintf(cmd, "sort -n -m -o all_sorted.txt %s", files);
      system(cmd);

      return(0);
}
