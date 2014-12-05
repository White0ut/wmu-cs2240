#include "server.h"

/***********************
*   RESPONSES 
************************/
static char* ok_response =
  "HTTP/1.0 200 OK\r\n"
  "Content-type: text/html\r\n\r\n"
  "%s";

static char* test_response = 
	"HTTP/1.0 200 OK\n"
  "Content-type: text/html\r\n\r\n"
  "<html><head><title>C Program</title></head>\n"
  "<body><h1>It Works!</h1></body>\n"
  "</html>\n";

static char* internal_error = 
	"HTTP/1.0 500 Internal Server Error\r\n"
	"Content-type: text/html\r\n\r\n"
  "<html>\n"
  " <body>\n"
  "  <h1>500 Internal Server Error</h1>\n"
  "  <p>The server could not handle your request.</p>\n"
  " </body>\n"
  "</html>\n";

static char* not_found_response_template = 
  "HTTP/1.0 404 Not Found\r\n"
  "Content-type: text/html\r\n\r\n"
  "\n"
  "<html>\n"
  " <body>\n"
  "  <h1>404 Not Found</h1>\n"
  "  <p>The requested URL %s was not found on this server.</p>\n"
  " </body>\n"
  "</html>\n";





/**********************************
*    PROCESS HTTP LINE
***********************************/
void handle_get(map_t map, int fd) {
	const char *url = map_get(&map, "URL");
	FILE *file_p;
	char *contents;
	size_t file_size = 0;
	char response[1024];

	// IF FILE IS "/"
	if (strlen(url) <= 1) {	
		//printf("Redirecting to index.html\n");

    // IF index.html doesn't exist
		if ((file_p = fopen("index.html", "r")) == NULL) {
			snprintf(response, sizeof(response), 
				not_found_response_template, "index.html");
			//printf("Serving index.html\n");
			write(fd, response, strlen(response));
			return;

		} else {

  		// Determine file_size
  		fseek(file_p, (long) 0, SEEK_END);
  		file_size = ftell(file_p);
  		fseek(file_p, (long) 0, SEEK_SET);

  		contents = (char*)malloc(file_size + 1);

  		size_t size = fread(contents, 1, file_size, file_p);
  		contents[size] = 0;

  		snprintf(response, sizeof(response), ok_response, contents);

  		fclose(file_p);
  		free(contents);

  		//printf("Serving %s\n", url);
  		write(fd, response, strlen(response));
  		return;
		}
	}

	// IF FILE IS SUPPLIED
	if ((file_p = fopen(url + 1,"r")) == NULL) {
		snprintf(response, sizeof(response), 
			not_found_response_template, url);
		//printf("404 File Not Found\n");
		write(fd, response, strlen(response));
		return;
	
	} else {
		// Determine file_size
		fseek(file_p, (long) 0, SEEK_END);
		file_size = ftell(file_p);
		fseek(file_p, (long) 0, SEEK_SET);

		contents = (char*)malloc(file_size + 1);

		size_t size = fread(contents, 1, file_size, file_p);
		contents[size] = 0;

		snprintf(response, sizeof(response), ok_response, contents);

		fclose(file_p);
		free(contents);
	}
	//printf("Serving %s\n",url);
	write(fd, response, strlen(response));
	return;
}




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
	char buffer[BUFFSIZE];
	size_t bytes_read;
	char method[BUFFSIZE];
	char url[BUFFSIZE];
	char protocol[BUFFSIZE];

	bytes_read = read(request_fd, buffer, BUFFSIZE - 1);
	if (bytes_read > 0) {
		buffer[bytes_read] = '\0';
		sscanf(buffer, "%s %s %s", method, url, protocol);

		//printf("%s\n",url);
		map_put(&resources_map, "URL", url);
		map_put(&resources_map, "METHOD", method);
		map_put(&resources_map, "PROTOCOL", protocol);

		if (strncmp(method, "GET", 3) == 0) {
				handle_get(resources_map, request_fd);
		} else {
			//printf("500 Internal Error\n");
			write(request_fd, internal_error, strlen(internal_error));
		}

	}


	map_destroy(&resources_map);
	close(request_fd);
}






/**********************************
*            MAIN
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

	if (argc < 2) {
		fprintf(stdout, "No port supplied: Default = 1234\n");
	} else {
		port = atoi(argv[1]);		
	}


	// SET SIGCHLD HANDLER
  sa.sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
	  perror("sigaction");
	  exit(1);
  }

	// Create TCP/IP socket
	if ((sfd= socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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
		//printf("=>");
		request_fd = accept(sfd, (struct sockaddr *)&cli_addr, &cli_len);
		
		if (request_fd < 0) {
			fprintf(stdout, "Error on Accept\n");
			continue;
		} else {
			//fprintf(stdout, "Accepted connection\n");
		}

		// fork
		pid = fork();

		if (pid < 0) {
			printf("Error on fork\n");
		}

		// CHILD PROCESS
		if (pid == 0) {
			close(sfd);
			handle_request(request_fd);
			exit(0);

		// PARENT PROCESS
		} else {
			close(request_fd);
		}
	}
	// UNREACHABLE
	return 0;

}
