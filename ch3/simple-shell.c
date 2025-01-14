/**
 * Simple shell interface program.
 *
 * Operating System Concepts - Tenth Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE		80 /* 80 chars per line, per command */

/* #define DEBUG */

#ifdef DEBUG
# define DBG_PRINTF(...) printf(__VA_ARGS__); fflush(stdout)
#else
# define DBG_PRINTF(...)
#endif

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
    	char *next_args[MAX_LINE/2 + 1];
	int should_run = 1;
		
	size_t size = MAX_LINE+1; 
	ssize_t nread;
	//char *line = (char *)malloc(sizeof(char)*size);
	char *line = NULL;
	//char *line_last =(char *)malloc(sizeof(char)*size);
	char line_buffer[size];
	char line_last[size];

	int cnt = 0;
	int cid;
	int has_history = 0;
	
    	while (should_run){   
		cnt ++;
		fflush(stdout);
        	DBG_PRINTF("\n\nosh(%d) pid(%d)\n", cnt, getpid()); 
		printf("osh>");
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
		int status;
		int filedesc;
		int redirect = 0;
		int redirect_flag = 0;
		int pipe_flag = 0;
		int next_cmd = 0;

		strcpy(line_buffer,line);
		len_args = osh_split_line(line_buffer, args);
		
		/* empty input */
		if(len_args == 0)
			continue;
		
		/* exit */
		else if(strcmp(args[0], "exit") == 0)
		{
			should_run = 0;
			break;
		}

		/* history */
		else if(strcmp(args[0], "!!") == 0)
		{
			if(!has_history)
			{
				printf("No commands in history.\n");
				continue;
			}
			printf("%s", line_last);
			strcpy(line_buffer, line_last);
			len_args = osh_split_line(line_buffer, args);
		}

		/* normal command, save history */
		else
		{
			strcpy(line_last, line);
			has_history = 1;
		}
			
		/* check IO redirection 
		 * now "<" ">" must be separated by spaces */
		for(int i = 0 ; i < len_args; ++i)
		{
			if(strcmp(args[i], ">") == 0)
			{
				redirect_flag = 1;
				redirect = STDOUT_FILENO;
				args[i] = NULL;
				filedesc = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
				break;
			}
			else if(strcmp(args[i], "<") == 0)
			{
				redirect_flag = 1;
				redirect = STDIN_FILENO;
				args[i] = NULL;
				filedesc = open(args[i+1], O_RDONLY); /* lack error handing */
				break;
			}
			else if(strcmp(args[i], "|") == 0)
			{
				pipe_flag = 1;
				next_cmd = i + 1;
				args[i] = NULL;
			}
		}

		/* check '&' symbol
		 * now it must be separated by spaces */
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
			DBG_PRINTF("child process id %d\n", getpid()); 
			DBG_PRINTF("pipe_flag %d\n", pipe_flag);
			fflush(stdout);
			if(redirect_flag)
				dup2(filedesc, redirect);
			if(pipe_flag)
			{
				int pipefd[2];
				if (pipe(pipefd) == -1)
				{
					perror("pipe");
					exit(EXIT_FAILURE);
				}
				
				int pid2 = fork();
				if(pid2 < 0)
				{
					perror("Fork Failed");
					return(1);
				}
				else if(pid2 > 0)
				{
					close(pipefd[1]);
					wait(NULL);
					DBG_PRINTF("before second cmd get inputs from pipe\n");
					dup2(pipefd[0], STDIN_FILENO);
					if(execvp(args[next_cmd], args + next_cmd) == -1)
					{	
						printf("%s: command not found\n", args[0]);
						return 1;
					}
				}
				else
				{	
					close(pipefd[0]);
					DBG_PRINTF("before first cmd sends outputs to pipe\n");
					fflush(stdout);
					dup2(pipefd[1], STDOUT_FILENO);
					if(execvp(args[0], args) == -1)
					{	
						printf("%s: command not found\n", args[0]);
						return 1;
					}
				}
			}

			/* single command, no pipe */
			else if(execvp(args[0], args) == -1)
			{	
				printf("%s: command not found\n", args[0]);
				return 1;
			}
		}

		else /* parent process */
		{
			if(!wait_flag)
				/* should kill zombies */
				continue;
			cid = waitpid(pid, &status, WUNTRACED|WCONTINUED);
			if(redirect)
				close(filedesc);
			
			DBG_PRINTF("Child process %d terminates with status %d (osh %d fork %d)\n",\
					cid, status, cnt, pid);
		}
    	}
    	
	return 0;
}
