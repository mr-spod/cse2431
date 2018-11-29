#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shellB.h"

void readHistory(char *commandHistory[10][MAXLINE/2+1], int *commandCount) {
  int i, j;
  FILE *f;
  char line[MAXLINE/2+1], newline[5];
  size_t len = 0;
  ssize_t read;

  f = fopen("commandHistory.txt", "r");
  if (f != NULL) {
    printf("file opened\n");
    fscanf(f, "%[^\n]", line);
    fscanf(f, "\n", newline);
    printf("read line: %s\n", line);
    *commandCount = atoi(line);
    printf("got command count: %d\n", *commandCount);
    i = *commandCount - 9;
    if (i < 1) i = 1;
    for (i; i <= *commandCount; i++) {
      line = malloc(MAXLINE/2+1);
      printf("scanning for index %d\n", i);
      fscanf(f, "%[^\n]", line);
      printf("scanned line: %s", line);
      fscanf(f, "\n", newline);
      strcpy(*commandHistory[(i % 10) - 1], line);
      printf("stored a line: %s\n", *commandHistory[(i % 10) - 1]);
      free(line);
    }
    fclose(f);
  }
  printf("done reading");
}

void writeHistory(char commandHistory[10][MAXLINE/2+1], int commandCount) {
  FILE *f;
  int i;
  char *command;
  f = fopen("commandHistory.txt", "w");
  printf("writing...");
  if (f != NULL) {
    fprintf(f, "%d\n", commandCount);
    i = 1;
    if (commandCount > 10) {
      i = commandCount - 9;
    }
    for (i; i <= commandCount; i++) {
      command = commandHistory[(i % 10) - 1];
      printf("writing %s", command);
      fprintf(f, "%s\n", command);
    }
  }
  fclose(f);
}
