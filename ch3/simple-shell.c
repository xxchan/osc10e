/**
 * Simple shell interface program.
 *
 * Operating System Concepts - Tenth Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE		80 /* 80 chars per line, per command */

int osh_split_line(char *line, char *args[])
{
	char *arg;
	int i;
	
	arg = strtok(line, " \t\r\n");
	i = 0;
	while (arg != NULL)
	{
		args[i++] = arg;
		arg = strtok(NULL, " \t\r\n");
	}
	args[i] = NULL;
	return i;
}


int main(void)
{
	char *args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
    	int should_run = 1;
		
	size_t size = MAX_LINE+1; 
	ssize_t nread;
	char *line = (char *)malloc(size * sizeof(char));
	char *line_last = (char *)malloc(size * sizeof(char));

	int cnt = 0;
	int cid;
    	while (should_run){   
		cnt ++;
        	printf("osh(%d)>", cnt);
        	fflush(stdout);
        
        	/**
         	 * After reading user input, the steps are:
         	 * (1) fork a child process
         	 * (2) the child process will invoke execvp()
         	 * (3) if command included &, parent will invoke wait()
         	 */
		
		/* read user input */
		nread = getline(&line, &size, stdin);
		if(nread < 0)
		{
			perror("Getline Failed");
			exit(EXIT_FAILURE);
		}
		int len_args;
		int wait_flag = 1;
		int has_history = 0;
		int status;

		len_args = osh_split_line(line, args);
		
		/* empty input */
		if(len_args == 0)
			continue;
		
		/* exit */
		if(strcmp(args[0], "exit") == 0)
		{
			should_run = 0;
			break;
		}

		/* check '&' symbol */
		if(strcmp(args[len_args-1], "&") == 0)
		{
			wait_flag = 0;
			args[len_args-1] = NULL;
		}

		pid_t pid;	
		pid = fork();
		if(pid < 0)
		{
			perror("Fork Failed");
			return(1);
		}
		else if(pid == 0) /* child process */
		{
			printf("child process id %d\n", getpid());
			fflush(stdout);
			if(execvp(args[0], args) == -1)
			{	
				printf("%s: command not found\n", args[0]);
				return 1;
			}
		}
		else /* parent process */
		{
			if(!wait_flag)
				/* should fix */
				continue;
			cid = waitpid(pid, &status, WUNTRACED|WCONTINUED);
			printf("Child process %d terminates (osh %d fork %d)\n", cid, cnt, pid);
		}
    	}
    	
	free(line);
	return 0;
}
