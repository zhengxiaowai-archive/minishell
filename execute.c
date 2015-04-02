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
	/*����ض���>*/

	
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
		//����һ�����������ض����null�ļ�
		if(cmd[i].infd == 0 && backgnd == 1)
		{
			cmd[i].infd = open("/dev/null", O_RDONLY);
		}
		//����һ����������Ϊ�鳤
		if(i == 0)
			setpgid(0,0);
		if(cmd[i].infd != 0)
		{
			close(0);
			dup(cmd[i].infd);//�ѱ�׼����رգ�Ȼ����һ���ܵ�д�˵�0
		}
		
		if(cmd[i].outfd != 1)
		{
			close(1);
			dup(cmd[i].outfd);//�ѱ�׼����رգ�Ȼ����һ���ܵ����˵�1�������������������;����ܵ���
		}
			
		int j;
		for (j =3; j < OPEN_MAX; ++j)
			close(j);
		
		//ǰ̨��ҵ�ܹ�����SIGINT,SIGQUIT�ź�
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
		
	//��Ϊ��̨��ҵ�������wait�ȴ��ӽ����˳������⽩������
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
		//ǰ̨��ҵ����Ҫ�ȴ��ܵ������һ�������˳�
		while(wait(NULL) != lastpid);
	}
		
	return 0;
}
