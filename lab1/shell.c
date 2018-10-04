#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
FOR GRADER:
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

int main(void)
{
    char inBuffer[MAXLINE]; /* Input buffer  to hold the command entered */
    char *args[MAXLINE/2+1];/* Command line arguments */
    int bkgd;             /* Equals 1 if a command is followed by '&', else 0 */
    int status;
    pid_t pid;


    while (1){            /* Program terminates normally inside setup */

	    bkgd = 0;
	    printf("COMMAND-> ");  /* Shell prompt */
      fflush(0);

      setup(inBuffer, args, &bkgd);       /* Get next command */

	/* Fill in the code for these steps:
	 (1) Fork a child process using fork(),
	 (2) The child process will invoke execvp(),
	 (3) If bkgd == 0, the parent will wait,
		o/w returns to the setup() function. */

      if ((pid = fork()) < 0) {
        printf("ERROR: problem forking a child process\n");
      } else if (pid == 0) {
        if (execvp(*args, args) < 0) {
          printf("ERROR: exec command failed\n");
        }
      } else if (bkgd == 0) {
        while (wait(&status) != pid) {
          printf("waiting...\n");
        }
        printf("parent process finished waiting\n");
      }
    }
}
