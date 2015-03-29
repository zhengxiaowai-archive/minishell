#ifndef __EXTERN_H__
#define __EXTERN_H__
#include "def.h"

extern char cmdline[MAXLINE + 1];
extern char avline[MAXLINE + 1];
extern char *lineptr;
extern char *avptr;
extern char infile[MAXLINE + 1];
extern char outfile[MAXLINE + 1];
extern int cmd_count;
extern int backgnd;
extern int append;

COMMAND cmd[PIPELINE];

#endif //__EXTERN_H__
