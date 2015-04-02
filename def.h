#ifndef __DEF_H__
#define __DEF_H__

#define MAXLINE 1024
#define MAXARG 20
#define PIPELINE 5
#define MAXNAME 100 
#define OPEN_MAX 1024
typedef struct command
{
	char *args[MAXARG + 1];
	int infd;
	int outfd;
}COMMAND;



#endif //__DEF_H__

