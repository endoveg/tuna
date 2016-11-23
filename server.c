#include <pthread.h>
#include <stdio.h>


int main()
{
	int S_fd = open("S.txt", S_IRWXU);
	int S[4][4];
	for (int i = 0; i < 4; i++)
	{
		
	}
	
	close(S_fd);
	return 0;
}
