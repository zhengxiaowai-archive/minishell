#include "extern.h"
#include "parse.h"
#include "init.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>

void get_command(int i);
int check(const char *str);
void getname(char *name);

void shell_loop()
{
	while(1)
	{
		printf("[minishell]$ ");
		fflush(stdout);
		init();
		if(-1 == read_command())
			break;
		parse_command();
		execute_comand();
	}

	printf("\n--byebye--\n");

}

int read_command()
{
	if(NULL == fgets(cmdline, MAXLINE, stdin))
		return -1;
	return 0;
}

int parse_command()
{
	get_command(0);

	/*判断是否有输入重定向符*/
	if(check("<"))
		getname(infile);

	/*判断是否有管道*/
	int i;
	for(i = 1;  i<PIPELINE; i++)
	{
		if(check("|"))
			get_command(i);
		else
			break;
	}

	/*判定时候有输出重定向符*/
	if(check("<"))
		getname(outfile);

	/*判定是否后台作业*/
	if(check("&"))
		backgnd = 1;

	/*判定命令结束*/
	if(check("\n"))
	{
		cmd_count = i;
		return cmd_count;
	}
	else
	{
		fprintf(stderr, "Command line syntax error");
		return -1;
	}

	return 0;


}

int execute_comand()
{
	/* pid_t pid = fork();
	if(pid < 0)
	{
		perror("fork failed");
		return -1;
	}
	else if(pid == 0)
		execvp(cmd.args[0], cmd.args);

		wait(NULL); */
	return 0;
}

/*提取命令*/
void get_command(int i)
{
	int j = 0;
	int inword;
	while(*lineptr != '\0')
	{
		/*除去空格*/
		while(*lineptr == ' ' || *lineptr == '\t')
			(*lineptr)++;
		
		cmd[i].args[j] = avptr;
		
		while(*lineptr != '\0' 
			&& *lineptr != ' '
			&& *lineptr != '\t'
			&& *lineptr != '>'
			&& *lineptr != '<'
			&& *lineptr != '|'
			&& *lineptr != '&'
			&& *lineptr != '\n')
		{
			*avptr++ = *lineptr++;
			inword = 1;
		}
		*avptr++ = '\0';
		switch (*lineptr)
		{
			case ' ':
			case '\t':
				inword = 0;
				j++;
				break;
			case '<':
			case '>':
			case '|':
			case '&':
			case '\n':
				if(inword == 0)
					cmd[i].args[j] = NULL;
				return;
			default:
				return;
		}
	}
}

int check(const char *str)
{
	return 0;
}

void getname(char *name)
{
	
}












