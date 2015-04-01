#include "extern.h"
#include "parse.h"
#include "init.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <fcntl.h>

#define OPEN_MAX 1024

void get_command(int i);
int check(const char *str);
void getname(char *name);
void print_command();
void forkexec(int i);

void shell_loop()
{
	while(1)
	{
		printf("[minishell]$ ");
		fflush(stdout);
		init();
		if(-1 == read_command())
		{
			perror("read failed");
			break;
		}
		parse_command();
		print_command();
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
	if(check("\n"))
		return 0;
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
	if(check(">"))
	{
		if(check(">"))
			append = 1;
		getname(outfile);
	}
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
		
		/*ls | grep init | wc -w*/
		if(infile[0] != '\0')
		{
			cmd[0].infd = open(infile, O_RDONLY);
		}
		if(outfile[0] != '\0')
		{
			if(append)
				cmd[cmd_count - 1].outfd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 00666 );
			else
				cmd[cmd_count - 1].outfd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 00666 );
		}	
		
		//因为后台作业不会调用wait等待子进程退出，避免僵死进程
		if(backgnd == 1)
		{
			signal(SIGCHLD, SIG_IGN);
		}
		
		int i = 0;
		int fd = 0;
		int fds[2];
		for(i =0; i < cmd_count; ++i)
		{
			if(i < cmd_count - 1)
			{
				pipe(fds);
				cmd[i].outfd = fds[1];
				cmd[i + 1].infd = fds[0];
			}
			
			forkexec(i);
			
			if((fd = cmd[i].infd) != 0)
			{
				close(fd);
			}
			if((fd = cmd[i].outfd) != 1)
			{
				close(fd);
			}
			
		}
		
		if(backgnd == 0)
		{
			//前台作业，需要等待管道中最后一个命令退出
			while(wait(NULL) != lastpid);
		}
		
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
		while (*lineptr == ' ' || *lineptr == '\t')
			lineptr++;
		
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
	char *p;
	while(*lineptr == ' ' || *lineptr == '\t')
		lineptr++;
	
	p = lineptr;
	while(*str != '\0' && *str == *p)
	{
		str++;
		p++;
	}
	
	if(*str == '\0')
	{
		lineptr = p;  //lineptr要移动过匹配的字符串
		return 1;
	}
	
	/*lineptr不变*/
	return 0;
}

void getname(char *name)
{
	while(*lineptr == ' ' || *lineptr == '\t')
		lineptr++;
	
	while(*lineptr != '\0'
			&& *lineptr != ' '
			&& *lineptr != '\t'
			&& *lineptr != '>'
			&& *lineptr != '<'
			&& *lineptr != '|'
			&& *lineptr != '&'
			&& *lineptr != '\n')
			{
				*name++ = *lineptr++;
			}
			
			*name = '\0';
}

void print_command()
{
	int i;
	int j;
	printf("cmd_count = %d\n", cmd_count);
	if(infile[0] != '\0')
		printf("infile = [%s]\n", infile);
	if(outfile[0] != '\0')
		printf("outfile = [%s]\n", outfile);
	
	for(i = 0; i < cmd_count; ++i)
	{
		j = 0;
		while(cmd[i].args[j] != NULL)
		{
			printf("[%s]", cmd[i].args[j]);
			j++;
		}
		printf("\n");
	}
}

void forkexec(int i)
{
	/*输出重定向>*/

	
	/*>*/
	pid_t pid;
	pid = fork();
	if(pid < 0)
	{
		perror("fork failed");
		return;
	}
	else if(pid > 0)
	{
		lastpid = pid;
	}
	else
	{
		//将第一条将简单命令重定向带null文件
		if(cmd[i].infd == 0 && backgnd == 1)
		{
			cmd[i].infd = open("/dev/null", O_RDONLY);
		}
		//将第一个简单命令做为组长
		if(i == 0)
			setpgid(0,0);
		if(cmd[i].infd != 0)
		{
			close(0);
			dup(cmd[i].infd);//把标准输入关闭，然后复制一个管道写端到0
		}
		
		if(cmd[i].outfd != 1)
		{
			close(1);
			dup(cmd[i].outfd);//把标准输出关闭，然后复制一个管道读端到1，这样程序的输入输出就经过管道了
		}
			
		int j;
		for (j =3; j < OPEN_MAX; ++j)
			close(j);
		
		//前台作业能够接收SIGINT,SIGQUIT信号
		if(backgnd == 0)
		{
			signal(SIGINT, SIG_DFL);
			signal(SIGQUIT, SIG_DFL);
		}
		execvp(cmd[i].args[0], cmd[i].args);
		perror("execvp failed\n");
		return;
	}
}








