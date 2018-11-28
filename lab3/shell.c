#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
FOR CSE 2431 GRADER:
Name: Sean O'Donnell
Lab 1
To compile: run `gcc -g shell.c -0 mysh`
To run compiled program: `mysh` or `./mysh`
*/

#define MAXLINE 80 /* 80 chars per line, per command, should be enough. */

/** The setup() routine reads in the next command line string storing it in the input buffer.
The line is separated into distinct tokens using whitespace as delimiters.  Setup also
modifies the args parameter so that it holds points to the null-terminated strings which
are the tokens in the most recent user command line as well as a NULL pointer, indicating the
end of the argument list, which comes after the string pointers that have been assigned to
args. ***/

void setup(char inBuffer[], char *args[],int *bkgd)
{
    int length,  /* #  characters in the command line */
        start,   /* Beginning of next command parameter           */
        i,       /* Index for inBuffer arrray          */
        j;       /* Where to place the next parameter into args[] */

    /* Read what the user enters */
    length = read(STDIN_FILENO, inBuffer, MAXLINE);

    start = -1;
    j = 0;

    if (length == 0)
        exit(0);            /* Cntrl-d was entered, end of user command stream */

    if (length < 0){
        perror("error reading command");
	exit(-1);           /* Terminate with error code of -1 */
    }

    /* Examine every character in the input buffer */
    for (i = 0; i < length; i++) {

        switch (inBuffer[i]){
        case ' ':
        case '\t' :          /* Argument separators */

            if(start != -1){
                args[j] = &inBuffer[start];    /* Set up pointer */
                j++;
            }

            inBuffer[i] = '\0'; /* Add a null char; make a C string */
            start = -1;
            break;

        case '\n':             /* Final char examined */
            if (start != -1){
                args[j] = &inBuffer[start];
                j++;
            }

            inBuffer[i] = '\0';
            args[j] = NULL; /* No more arguments to this command */
            break;

        case '&':
            *bkgd = 1;
            inBuffer[i] = '\0';
            break;

        default :             /* Some other character */
            if (start == -1)
                start = i;
	}

    }
    args[j] = NULL; /* Just in case the input line was > 80 */
}

static char commandHistory[10][MAXLINE/2+1];
static int historyIndex, isHistory, isR;

int main(void)
{
    char inBuffer[MAXLINE]; /* Input buffer  to hold the command entered */
    char *args[MAXLINE/2+1];/* Command line arguments */
    char rr[50], r[50], history[50], h[50], firstTwo[50], first[50], command[MAXLINE/2+1];
    char **historyArgs;
    char *historyCommand;
    int commandCount;
    int i, j;
    FILE *f;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int lowestHistoryNum;

    int bkgd;             /* Equals 1 if a command is followed by '&', else 0 */
    int status;
    pid_t pid;

    historyIndex = -1;
    isHistory = 0;
    isR = 0;
    commandCount = 0;
    strcpy(rr, "rr");
    strcpy(history, "history");
    strcpy(h, "h");

    f = fopen("commandHistory.txt", "r");
    if (f != NULL) {
      read = getline(&line, &len, f);
      if (read == -1) exit(EXIT_FAILURE);
      commandCount = atoi(line);
      i = commandCount - 9;
      if (i < 1) i = 1;
      for (i; i <= commandCount; i++) {
        read = getline(&line, &len, f);
        historyIndex = (i % 10) - 1;
        strcpy(commandHistory[historyIndex], line);
      }
      fclose(f);
      if (line) free(line);
    }

    while (1){            /* Program terminates normally inside setup */
	    bkgd = 0;
      i = 1;
      historyIndex = -1;

	    printf("\n\nCOMMAND-> ");  /* Shell prompt */
      fflush(0);

      setup(inBuffer, args, &bkgd);       /* Get next command */

      if ((pid = fork()) < 0) { // call fork to get child process
        printf("ERROR: problem forking a child process\n");
      } else if (pid == 0) { // child process executes the command
        sprintf(first, "%s", *args);
        if ((strcmp(history, first) == 0) || (strcmp(h, first) == 0)) {
          /* Print history */
          isHistory = 1;
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
          historyIndex = -1;
          if (commandCount > 10) {
            i = commandCount - 9;
          }
          sprintf(firstTwo, "%s %s", args[0], args[1]);
          for (i; i <= commandCount; i++) {
            sprintf(r, "r %d", i);
            if (strcmp(r, firstTwo) == 0) {
              historyIndex = (i % 10) - 1;
            }
          }

          if (historyIndex >= 0) {
            isR = 1;
            if (commandCount > 0) {
              printf("command: %s\n", commandHistory[historyIndex]);
              system(commandHistory[historyIndex]);
            }
          } else if (strcmp(rr, first) == 0) {
            isR = 1;
            /* Most recent command */
            if (commandCount > 0) {
              historyIndex = (commandCount % 10) - 1;
              system(commandHistory[historyIndex]);
            }
          } else if (execvp(*args, args) < 0) {
            printf("ERROR: exec command failed\n");
          }
        }

      } else if (bkgd == 0) { // parent process waits if bkgd == 0
        while (wait(&status) != pid) { // wait until the wait function returns the parent pid returned from fork
          printf("waiting...\n");
        }
      }

      if (isHistory == 0) {
        commandCount++;
        if (isR != 0) {
          strcpy(commandHistory[(commandCount % 10) - 1], commandHistory[historyIndex]);
          isR = 0;
        } else {
          i = 0;
          historyCommand = args[i];
          strcpy(command, historyCommand);
          i++;
          historyCommand = args[i];
          while (historyCommand != NULL) {
            strcat(command, " ");
            strcat(command, historyCommand);
            i++;
            historyCommand = args[i];
          }
          strcpy(commandHistory[(commandCount % 10) - 1], command);
        }
        
        f = fopen("commandHistory", "w");
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
      isHistory = 0;
    }
}
