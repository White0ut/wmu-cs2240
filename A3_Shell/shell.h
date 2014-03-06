#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>   // OverKill 
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int makeargv(char *s, char *delimiters, char ***argvp);
void main();

#endif 
