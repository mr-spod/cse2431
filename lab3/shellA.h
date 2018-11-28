#define MAXLINE 80 /* 80 chars per line, per command, should be enough. */
void executeShellCommand(char *args[], int *isHistory, int *isR, int *historyIndex, int commandCount, char commandHistory[10][]);
