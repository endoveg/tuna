#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>


int Fork(int N, int pid)
{
	for (int i = 1; i <= N; i++)
	{
		if (fork() == -1)
			return -1;
		if (pid != getpid())
		{
			printf("PID = %d \t PPID = %d \n", getpid() , getppid());
			break;
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	if (argc < 2)
		return -1;
		
	int N = atoi(argv[1]);
	int pid = getpid();
	
	if (Fork(N, pid) == -1)
		return -1;
	
	if (pid == getpid())
	{
		int status;
		while(waitpid(-1, &status, 0) > 0) {};
		printf("status = %d\n", WEXITSTATUS(status));
	}
	return 0;
}
