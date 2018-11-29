#define MAXLINE 80 /* 80 chars per line, per command, should be enough. */
void readHistory(int *cmdCount);

void writeHistory(char cmdHistory[10][MAXLINE/2+1], int cmdCount);
