#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "map/map.h"

#define BUFFSIZE 512

int main(int argc, char *argv[]);

void handle_request(int request_fd);

void handle_get(map_t map, int fd);

void throw_five(int fd);