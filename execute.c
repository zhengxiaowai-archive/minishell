#include "extern.h"
#include "parse.h"
#include "init.h"
#include "execute.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <fcntl.h>
#include <stdlib.h>
int forkexec(int i)
{
	/*输出重定向>*/

	
	/*>*/
	pid_t pid;
	pid = fork();
	if(pid < 0)
	{
		perror("fork failed");
		return -1;
	}
	else if(pid > 0)
	{
		if(backgnd == 1)
			printf("%d\n", pid);
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
		if(execvp(cmd[i].args[0], cmd[i].args) == -1)
		{
			perror("execvp failed");
			exit(0);
		}
	}
	return 0;
}


int execute_disk_command(void)
{
	if(cmd_count == 0)
		return -1;

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
	else
		signal(SIGCHLD,SIG_DFL);
		
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
			
		if(forkexec(i) == -1)
			return -1;
			
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
