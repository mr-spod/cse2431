#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shellB.h"
#include "shell.h"

/*
Author: Sean O'Donnell
See shell.c for compile/run instructions
*/

/* Remove the hanging newline char at the end of an fgets read */
void scrubNewline(char *line) {
    line[strlen(line) - 1] = '\0';
}

void readHistory(int *cmdCount) {
  int i, j;
  FILE *f;
  char *line;
  size_t len = 0;
  ssize_t read;

  line = malloc(MAXLINE/2+1 * sizeof(char));
  f = fopen("commandHistory.txt", "r");
  if (f != NULL) {
    /* first line of file is the # of commands elapsed */
    fgets(line, MAXLINE/2+1, f);
    scrubNewline(line);
    /* update passed pointer value */
    *cmdCount = atoi(line);
    free(line);
    i = *cmdCount - 9;
    if (i < 1) i = 1;
    for (i; i <= *cmdCount; i++) {
      /* store each of the following lines in the file into commandHistory */
      line = malloc(MAXLINE/2+1 * sizeof(char));
      fgets(line, MAXLINE/2+1, f);
      scrubNewline(line);
      strcpy(commandHistory[(i % 10) - 1], line);
      free(line);
    }
    fclose(f);
  }
}

void writeHistory(int cmdCount) {
  FILE *f;
  int i;
  char *command;
  f = fopen("commandHistory.txt", "w");
  if (f != NULL) {
    /* print the # of commands elapsed */
    fprintf(f, "%d\n", cmdCount);
    i = 1;
    if (cmdCount > 10) {
      i = cmdCount - 9;
    }
    for (i; i <= cmdCount; i++) {
      /* print the commands in order of oldest to newest */
      command = commandHistory[(i % 10) - 1];
      fprintf(f, "%s\n", command);
    }
  }
  fclose(f);
}
