#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shellA.h"
#include "shell.c"

void executeShellCommand(char *args[], int *isHistory, int *isR, int *historyIndex, int commandCount) {
  char rr[50], r[50], history[50], h[50], firstTwo[50], first[50];
  int i;
  char *historyCommand;

  strcpy(rr, "rr");
  strcpy(history, "history");
  strcpy(h, "h");

  sprintf(first, "%s", *args);
  if ((strcmp(history, first) == 0) || (strcmp(h, first) == 0)) {
    /* Print history */
    *isHistory = 1;
    i = 1;
    if (commandCount > 10) {
      i = commandCount - 9;
    }
    printf("\nHISTORY (starting at %d)\n", i);
    for (i; i <= commandCount; i++) {
      printf("%d ", i);
      historyCommand = commandHistory[(i % 10) - 1];
      printf("%s\n", historyCommand);
    }
  } else {
    /* Check if they typed r num */
    i = 1;
    *historyIndex = -1;
    if (commandCount > 10) {
      i = commandCount - 9;
    }
    sprintf(firstTwo, "%s %s", args[0], args[1]);
    for (i; i <= commandCount; i++) {
      sprintf(r, "r %d", i);
      if (strcmp(r, firstTwo) == 0) {
        *historyIndex = (i % 10) - 1;
      }
    }

    if (*historyIndex >= 0) {
      *isR = 1;
      if (commandCount > 0) {
        printf("command: %s\n", commandHistory[*historyIndex]);
        system(commandHistory[*historyIndex]);
      }
    } else if (strcmp(rr, first) == 0) {
      *isR = 1;
      /* Most recent command */
      if (commandCount > 0) {
        *historyIndex = (commandCount % 10) - 1;
        printf("command: %s\n", commandHistory[*historyIndex]);
        system(commandHistory[*historyIndex]);
      }
    } else if (execvp(*args, args) < 0) {
      printf("ERROR: exec command failed\n");
    }
  }

}
