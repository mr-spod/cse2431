/* Sean O'Donnell */

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MAX_COUNT 200

int main()
{
	pid_t pid;

	int i;

	fork();
	pid = getpid();

	for(i=1; i <= MAX_COUNT; i++)
		printf("*This line is from pid %d, value = %d\n", pid, i);
}
