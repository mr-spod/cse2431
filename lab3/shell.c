#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shellA.h"
#include "shellB.h"

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

static int historyIndex, isHistory, isR;

int main(void)
{
    char inBuffer[MAXLINE], command[MAXLINE/2+1]; /* Input buffer  to hold the command entered */
    char *args[MAXLINE/2+1];/* Command line arguments */
    char **historyArgs;
    char *historyCommand;
    int commandCount, i;
    FILE *f;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    /* Equals 1 if a command is followed by '&', else 0 */
    int bkgd;
    int status;
    pid_t pid;

    historyIndex = -1;
    isHistory = 0;
    isR = 0;
    commandCount = 0;

    readHistory(&commandCount);

    /* Program terminates normally inside setup */
    while (1){
	    bkgd = 0;
      i = 1;
      historyIndex = -1;

      /* Shell prompt */
	    printf("\n\nCOMMAND-> ");
      fflush(0);

      /* Get next command */
      setup(inBuffer, args, &bkgd);

      if ((pid = fork()) < 0) {
        /* call fork to get child process */
        printf("ERROR: problem forking a child process\n");
      } else if (pid == 0) {
        /* child process executes the command */
        executeShellCommand(args, &isHistory, &isR, &historyIndex, commandCount, commandHistory);
      } else if (bkgd == 0) {
        /* parent process waits if bkgd == 0 */
        while (wait(&status) != pid) {
          /* wait until the wait function returns the parent pid returned from fork */
          printf("waiting...\n");
        }
      }

      if (isHistory == 0) {
        /* Store the command in the history */
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

        writeHistory(commandHistory, commandCount);
      }
      isHistory = 0;
    }
}
