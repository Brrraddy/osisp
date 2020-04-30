#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/wait.h>

#define BUF_SIZE 4096

int maxProcessesCount, processesCount = 0;

void process_file(char *filePath, off_t fileSize)
{
	int file = open(filePath, O_RDONLY);
	if(file == -1){
		printf("Error: cannot open file %s.\n", filePath);
		exit(1);
	}
	char separatorFlag = 1;
	unsigned wordCount = 0;
	char buf[BUF_SIZE];
	int count = 0;
	while((count = read(file, &buf, sizeof(BUF_SIZE))) != 0){
		if(count == -1){
			printf("Error: cannot read block from file %s.\n", filePath);
			exit(1);
		}
		for(int i = 0; i < count; i++){
			switch(buf[i]){
				case ' ':
				case '\n':
				case '\t':
					separatorFlag = 1;
					break;
				default:
					if(separatorFlag == 1){ separatorFlag = 0; wordCount++; }
					break;
			}
		}
	}
	printf("Pid: %d   File: %s   Size: %d   Words: %d\n", getpid(), filePath, fileSize, wordCount);
	if(close(file) == -1){
		printf("Error: cannot close file, %s.\n", filePath);
		exit(-1);
	}
	exit(0);
}

void process_dir(char *dirPath)
{
	struct dirent dEnt;
	struct dirent *dEntPtr = NULL;
	DIR *dir = NULL;
	int retValue = 0;
	char filePath[PATH_MAX + 1]; // buffer
	if ((dir = opendir(dirPath)) == NULL)
	{
		printf("Error: cannot open directory %s.\n", dirPath);
		return;
	}
	retValue = readdir_r (dir, &dEnt, &dEntPtr);
	while (dEntPtr != NULL)
	{
		struct stat info;

		if(strcmp(dEnt.d_name, ".") == 0 || strcmp(dEnt.d_name, "..") == 0)
		{
			retValue = readdir_r (dir, &dEnt, &dEntPtr);
			continue;
		}
		strcpy(filePath, dirPath);
		if (strcmp(filePath, "/") != 0)
		{
			strcat(filePath, "/");
		}
		strcat(filePath, dEnt.d_name);
		
		if (lstat(filePath, &info) == 0)
		{
			if(S_ISDIR(info.st_mode)) // non-zero if the file is a directory 
			{
				process_dir(filePath);
			}
			else
			if(S_ISREG(info.st_mode)) // non-zero if the file is a regular file
			{int status;
				if (maxProcessesCount == processesCount)
				{
					processesCount--;
					wait(NULL);
				}
				int pid = fork();
				switch (pid)
				{
					case -1:
					{
						printf("Error: cannot create child process.");
						break;
					}
					case 0:
					{
						process_file(filePath, info.st_size);
						break;
					}
					default:
					{
						processesCount++;
						break;
					}
				}
			}
		}
		else
		{
			printf("Error: cannot get stat %s\n.", filePath);
			return;
		}
		
		retValue = readdir_r (dir, &dEnt, &dEntPtr);
	}
	if (retValue != 0)
	{
		printf("Error: cannot read next record in directory %s.\n", dirPath);
		return;
	}
	if (closedir(dir) == -1)
	{
		printf("Error: cannot close directory %s.\n", dirPath);
		return;
	}
}

int main(int argc, char *argv[])
{
	if (argc < 3) 
	{
		printf("Error: need more arguments.");
		exit(1);
	}
	if ((maxProcessesCount = atoi(argv[1])) < 1) 
	{
		printf("Error: need at least 1 child process.");
		exit(1);
	}
	char *path = NULL;
	if ((path = realpath(argv[2], NULL)) == NULL) //return the canonicalized absolute pathname
	{
		printf("Error: path doesn't exist.");
		exit(1);
	}
	process_dir(path);
	while(wait(NULL) != -1){printf("ex\n");}
	return 0;
}
