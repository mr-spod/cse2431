#include <sys/types.h>
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

char **commandHistory[10];
int historyIndex, isHistory, isR;

int main(void)
{
    char inBuffer[MAXLINE]; /* Input buffer  to hold the command entered */
    char *args[MAXLINE/2+1];/* Command line arguments */
    char rr[50], r[50], history[50], h[50], firstTwo[50], first[50];
    // char **commandHistory[10];
    char **historyArgs;
    char *historyCommand;
    int commandCount;
    int i, j;

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

    while (1){            /* Program terminates normally inside setup */

	    bkgd = 0;

      /* Print history */
      i = 1;
      if (commandCount > 10) {
        i = commandCount - 9;
      }
      printf("\nCURRENT STORED HISTORY\n");
      for (i; i <= commandCount; i++) {
        printf("%d", i);
        historyArgs = commandHistory[(i % 10) - 1];
        historyIndex = 0;
        historyCommand = historyArgs[historyIndex];
        while (historyCommand != NULL) {
          printf(" %s", historyCommand);
          historyIndex++;
          historyCommand = historyArgs[historyIndex];
        }
        printf("\n");
      }
      i = 1;
      historyIndex = -1;


	    printf("\n\nCOMMAND-> ");  /* Shell prompt */
      fflush(0);

      setup(inBuffer, args, &bkgd);       /* Get next command */
	/* Fill in the code for these steps:
	 (1) Fork a child process using fork(),
	 (2) The child process will invoke execvp(),
	 (3) If bkgd == 0, the parent will wait,
		o/w returns to the setup() function. */

      if ((pid = fork()) < 0) { // call fork to get child process
        printf("ERROR: problem forking a child process\n");
      } else if (pid == 0) { // child process executes the command
        if ((strcmp(history, first) == 0) || (strcmp(h, first) == 0)) {
          /* Print history */
          isHistory = 1;
          i = 1;
          if (commandCount > 10) {
            i = commandCount - 9;
          }
          printf("\nHISTORY (starting at %d)\n", i);
          for (i; i <= commandCount; i++) {
            printf("%d", i);
            historyArgs = commandHistory[(i % 10) - 1];
            historyIndex = 0;
            historyCommand = historyArgs[historyIndex];
            while (historyCommand != NULL) {
              printf(" %s", historyCommand);
              historyIndex++;
              historyCommand = historyArgs[historyIndex];
            }
            printf("\n");
          }
        } else {
          sprintf(first, "%s", *args);
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
          printf("history index: %d\n", historyIndex);
          printf("command count: %d\n\n", commandCount);

          if (historyIndex >= 0 && historyIndex < commandCount && historyIndex >= commandCount - 9) {
            printf("is r num\n");
            isR = 1;
            if (commandCount > 0) {
              printf("command: %s\n", *commandHistory[historyIndex]);
              execvp(*commandHistory[historyIndex], commandHistory[historyIndex]);
            }
          } else if (strcmp(rr, first) == 0) {
            printf("is rr");
            isR = 1;
            /* Most recent command */
            if (commandCount > 0) {
              historyIndex = (commandCount % 10) - 1;
              execvp(*commandHistory[historyIndex], commandHistory[historyIndex]);
            }
          } else if (execvp(*args, args) < 0) {
            printf("ERROR: exec command failed\n");
          }

        }

      } else if (bkgd == 0) { // parent process waits if bkgd == 0
        while (wait(&status) != pid) { // wait until the wait function returns the parent pid returned from fork
          printf("waiting...\n");
        }
        printf("parent process finished waiting\n");


      } // return to top of loop immediately if parent process & bkgd == 1
      printf("isHistory: %d", isHistory);
      if (isHistory == 0) {
        commandCount++;

        if (isR != 0) {
          printf("\n\nargs being stored: %s %s\n\n\n", *commandHistory[historyIndex], commandHistory[historyIndex][1]);

          commandHistory[(commandCount % 10) - 1] = commandHistory[historyIndex];
          printf("\n\nstored: %s %s\n\n\n", *commandHistory[(commandCount % 10) - 1], commandHistory[(commandCount % 10) - 1][1]);

          isR = 0;
        } else {
          printf("\n\nargs being stored: %s %s\n\n\n", *args, args[1]);

          commandHistory[(commandCount % 10) - 1] = args;
          printf("\n\nstored: %s %s\n\n\n", *commandHistory[(commandCount % 10) - 1], commandHistory[(commandCount % 10) - 1][1]);
        }
      }
      isHistory = 0;
    }
}
