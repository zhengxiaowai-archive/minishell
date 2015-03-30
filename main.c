#include "extern.h"
#include "parse.h"
#include "init.h"
#include "def.h"

char cmdline[MAXLINE + 1];
char avline[MAXLINE + 1];
char *lineptr;
char *avptr;

char infile[MAXLINE + 1];
char outfile[MAXLINE + 1];

int cmd_count;
int backgnd;
int append;
int lastpid;

COMMAND cmd[PIPELINE];
int main(int argc, char *argv[])
{
	setup();
	shell_loop();
	return 0;
}
