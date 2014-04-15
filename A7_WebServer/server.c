// Server.c ? maybe? :D
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

#define MAXMSG 512

/*
* Prototypes
*
*/
int main(int argc, char *argv[]);
void handle_request(int request_fd);
static void sigchld_handler(int sig);


/**********************************
*         SIGCHLD handler
***********************************/
static void sigchld_handler(int sig) {
	while(waitpid(-1, NULL, WNOHANG) > 0);
}


/**********************************
*     HTTP GET Request Handler
***********************************/
void handle_request(int request_fd) {
	map_t resources_map;
	map_init(&resources_map);
	int nbytes;
	char buffer[MAXMSG];

	nbytes = read(request_fd, buffer, MAXMSG);
	if (nbytes < 0) {
		perror("read");
		exit(-1);
	}



	map_destroy(&resources_map);
	close(request_fd);
}

/**********************************
*             Main
***********************************/
int main(int argc, char *argv[]) {
	// Initialize Variables
	int sfd;					// Socket fd
	int port = 1234;			// Port number
	int request_fd;				// Socket accepting request
	socklen_t cli_len;  		// Length of address struct
	struct sockaddr_in my_addr; // Address of my service
	struct sockaddr_in cli_addr;// Address of clients service
	int sockoptval = 1;
	int pid;
	struct sigaction sa;
	int i;

	if(argc < 2) {
		fprintf(stdout, "No port supplied: Default = 1234\n");
	} else {
		port = atoi(argv[1]);		
	}


	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	// Create TCP/IP socket
	if((sfd= socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Cannot create socket\n");
		exit(1);
	}

	/* set SO_REUSEADDR. 
	This allows us to reuse the port immediately after exit*/
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));

	// Zero out and fill in socket struct
	memset((char *)&my_addr, 0, sizeof(my_addr));	// zero out struct
	my_addr.sin_family = AF_INET;					// WE ARE FAMILY
	my_addr.sin_addr.s_addr = INADDR_ANY;	
	my_addr.sin_port = htons(port);

	if (bind(sfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
		fprintf(stderr, "Bind failed\n");
		exit(1);
	}

	// Set socket for listening
	listen(sfd, 5); 
	
	printf("Loopin and Dooping\n");

	// We loop, while accepting connections
	while(1) {
		printf("=>");
		request_fd = accept(sfd, (struct sockaddr *)&cli_addr, &cli_len);
		
		if(request_fd < 0) {
			fprintf(stdout, "Error on Accept\n");
		}else {
			fprintf(stdout, "Accepted connection\n");
		}

		pid = fork();

		if (pid < 0) {
			printf("Error on fork\n");
		}
		if (pid == 0) {
			close(sfd);
			handle_request(request_fd);
			exit(0);
		}else {
			close(request_fd);
		}

		
	}
	return 0;

}
