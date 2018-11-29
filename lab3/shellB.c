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
  char line[MAXLINE/2+1];
  size_t len = 0;
  ssize_t read;

  printf("attempting to open..\n");
  f = fopen("commandHistory.txt", "r");
  printf("called open\n");
  if (f != NULL) {
    printf("file opened\n");
    read = getline(&line, &len, f);
    if (read == -1) exit(EXIT_FAILURE);
    *commandCount = atoi(line);
    printf("got command count: %d\n", *commandCount);
    i = *commandCount - 9;
    if (i < 1) i = 1;
    for (i; i <= *commandCount; i++) {
      read = getline(&line, &len, f);
      strcpy(*commandHistory[(i % 10) - 1], line);
      printf("read a line: %s\n", *commandHistory[(i % 10) - 1]);
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
