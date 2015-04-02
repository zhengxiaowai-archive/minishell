#include "extern.h"
#include "parse.h"
#include "init.h"
#include "execute.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <fcntl.h>



void get_command(int i);
int check(const char *str);
void getname(char *name);
void print_command();


void shell_loop()
{
	while(1)
	{
		//初始化环境
		init();
		//读取命令
		if(-1 == read_command())
		{
			perror("read failed");
			break;
		}
		//解析命令
		parse_command();
		//print_command();
		//执行命令
		execute_comand();
	}

	printf("\n--byebye--\n");

}

int read_command()
{
	//读取命令，cmdline中包含\n
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
	//判定是否执行内部命令
	if(execute_disk_command() == -1)
		return -1;
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









