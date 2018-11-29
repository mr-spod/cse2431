#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shellB.h"

/* Remove the hanging newline char at the end of an fgets read */
void scrubNewline(char *line) {
    line[strlen(line) - 1] = '\0';
}

void readHistory(char *commandHistory[10][MAXLINE/2+1], int *commandCount) {
  int i, j;
  FILE *f;
  char *line;
  size_t len = 0;
  ssize_t read;

  line = malloc(MAXLINE/2+1 * sizeof(char));
  f = fopen("commandHistory.txt", "r");
  if (f != NULL) {
    printf("file opened\n");
    fgets(line, MAXLINE/2+1, f);
    scrubNewline(line);
    printf("read line: %s\n", line);
    *commandCount = atoi(line);
    free(line);
    printf("got command count: %d\n", *commandCount);
    i = *commandCount - 9;
    if (i < 1) i = 1;
    for (i; i <= *commandCount; i++) {
      line = malloc(MAXLINE/2+1 * sizeof(char));
      printf("scanning for index %d\n", i);
      fgets(line, MAXLINE/2+1, f);
      printf("got line: %s\n", line);
      scrubNewline(line);
      printf("scrubbed newline: %s\n", line);
      strcpy((*commandHistory)[(i % 10) - 1], line);
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
