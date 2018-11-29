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

void readHistory(char (*cmdHistory)[10][MAXLINE/2+1], int *cmdCount) {
  int i, j;
  FILE *f;
  char *line;
  char history[10][MAXLINE/2+1];
  size_t len = 0;
  ssize_t read;

  history = *cmdHistory;
  line = malloc(MAXLINE/2+1 * sizeof(char));
  f = fopen("cmdHistory.txt", "r");
  if (f != NULL) {
    printf("file opened\n");
    fgets(line, MAXLINE/2+1, f);
    scrubNewline(line);
    printf("read line: %s\n", line);
    *cmdCount = atoi(line);
    free(line);
    printf("got command count: %d\n", *cmdCount);
    i = *cmdCount - 9;
    if (i < 1) i = 1;
    for (i; i <= *cmdCount; i++) {
      line = malloc(MAXLINE/2+1 * sizeof(char));
      printf("scanning for index %d\n", i);
      fgets(line, MAXLINE/2+1, f);
      printf("got line: %s\n", line);
      scrubNewline(line);
      printf("scrubbed newline: %s\n", line);
      strcpy(history[(i % 10) - 1], line);
      printf("stored a line: %s\n", *cmdHistory[(i % 10) - 1]);
      free(line);
    }
    fclose(f);
  }
  *cmdHistory = history;
  printf("done reading");
}

void writeHistory(char cmdHistory[10][MAXLINE/2+1], int cmdCount) {
  FILE *f;
  int i;
  char *command;
  f = fopen("cmdHistory.txt", "w");
  printf("writing...");
  if (f != NULL) {
    fprintf(f, "%d\n", cmdCount);
    i = 1;
    if (cmdCount > 10) {
      i = cmdCount - 9;
    }
    for (i; i <= cmdCount; i++) {
      command = cmdHistory[(i % 10) - 1];
      printf("writing %s", command);
      fprintf(f, "%s\n", command);
    }
  }
  fclose(f);
}
