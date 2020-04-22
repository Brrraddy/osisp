#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/timeb.h>
#include <time.h>
int main()
{
	int status;
	pid_t child1, child2;
	if (child1 = fork() == 0)
	{
		printf("Child: %d\n", getpid());
		printf("Parent: %d\n", getpid());
	}
	else 
	{
		if (child2 = fork() == 0)
		{
			printf("Child: %d\n", getpid());
			printf("Parent: %d\n", getpid());
		}

		else
		{
			waitpid(child1, &status, 0);
			printf("Process (1) exit successfully: %s\n", (WIFEXITED(status) ? "true" : "false"));
		       		printf("child exitcode = %i\n", WEXITSTATUS(status));
			waitpid(child2, &status, 0);
			printf("Process (2) exit successfully: %s\n", (WIFEXITED(status) ? "true" : "false"));
		       		printf("child exitcode = %i\n", WEXITSTATUS(status));
			system("ps-x");
			printf("Main process: %d\n", getpid());
		}
	}
	struct timeb sys_time;
	struct tm *loctime;
	ftime(&sys_time);
	loctime = localtime(&sys_time.time);
	printf("%d:%d:%d:%d\n", loctime->tm_hour, loctime->tm_min, loctime->tm_sec, sys_time.millitm);
	return 0;
	
}
