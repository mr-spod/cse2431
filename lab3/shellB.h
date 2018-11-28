#define MAXLINE 80 /* 80 chars per line, per command, should be enough. */
void readHistory(char *commandHistory[10][MAXLINE/2], int *commandCount);

void writeHistory(char commandHistory[10][MAXLINE/2], int commandCount);
