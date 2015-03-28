#include "extern.h"
#include "init.h"
#include <signal.h>
#include <stdio.h>
#include <string.h>
void sigint_handler(int sig);

void setup()
{
	signal(SIGINT, sigint_handler);
	signal(SIGQUIT, SIG_IGN);
}

void sigint_handler(int sig)
{
	printf("\n[minishell]$ ");
	fflush(stdout);
}

void init()
{
	memset(cmd, 0, sizeof(cmd));
	memset(cmdline, 0, sizeof(cmdline));
	lineptr = cmdline;
	avptr = avline;
}
