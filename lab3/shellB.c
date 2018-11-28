#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shellB.h"

void readHistory(char *commandHistory[10][], int *commandCount) {
  int i, j;
  FILE *f;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  f = fopen("commandHistory.txt", "r");
  if (f != NULL) {
    read = getline(&line, &len, f);
    if (read == -1) exit(EXIT_FAILURE);
    *commandCount = atoi(line);
    i = *commandCount - 9;
    if (i < 1) i = 1;
    for (i; i <= *commandCount; i++) {
      read = getline(&line, &len, f);
      strcpy(*commandHistory[(i % 10) - 1], line);
    }
    fclose(f);
    if (line) free(line);
  }
}

void writeHistory(char commandHistory[10][], int commandCount) {
  FILE *f;
  int i;
  f = fopen("commandHistory.txt", "w");
  if (f != NULL) {
      fprintf(f, "%d\n", commandCount);
      i = commandCount - 9;
      if (i < 1) i = 1;
      for (i; i <= commandCount; i++) {
        fprintf(f, "%s\n", commandHistory[(i % 10) - 1]);
      }
  }
  fclose(f);
}
