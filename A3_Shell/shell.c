#include "shell.h"

/*
 * Make argv array (*arvp) for tokens in s which are separated by
 * delimiters.   Return -1 on error or the number of tokens otherwise.
 */
int makeargv(char *s, char *delimiters, char ***argvp)
{
   char *t;
   char *snew;
   int numtokens;
   int i;
    /* snew is real start of string after skipping leading delimiters */
   snew = s + strspn(s, delimiters);
                              /* create space for a copy of snew in t */
   if ((t = (char *)calloc(strlen(snew) + 1, sizeof(char))) == NULL) {
      *argvp = NULL;
      numtokens = -1;
   } else {                     /* count the number of tokens in snew */
      strcpy(t, snew);
      if (strtok(t, delimiters) == NULL)
         numtokens = 0;
      else
      	for (numtokens = 1; strtok(NULL, delimiters) != NULL;
              numtokens++);  
                
      if ((*argvp = (char**)calloc(numtokens + 1, sizeof(char **))) == NULL) {
         free(t);
         numtokens = -1;
      } else {            
         if (numtokens > 0) {
            strcpy(t, snew);
            **argvp = strtok(t, delimiters);
            
            for (i = 1; i < numtokens + 1; i++)
               *((*argvp) + i) = strtok(NULL, delimiters);
         } else {
           **argvp = NULL;
           free(t);
         }
      }
   }
   return numtokens;
}
		/*
Get the command line, forks it, runs first process
but it must fork each process separated by the pipes, redirects, etc
fork from the right to the left

int fdl[2] and int fdr[2] (file descriptor)

for the loop to iterate correctly fdl needs to sync up with fdr

            pipe
            fork
            dup2 and close on either end of the pipe shared between the parent and child
            exec in some fashion 

		*/

void main() 
{


	while (1)
	{

			// Variables
		char *input = NULL;
		char **pipeArgs;
		char **argPip;
		char cwd[1024];
		size_t input_size = 0;
		pid_t pid, shell_pid;
		int stat, lenArg;	

		// Retrieve PID of the parent process. 
		shell_pid = getpid();

		// Print shell prompt. 

		printf("{%i}$ ", shell_pid);
		// Retrieve input from stdin.
		input_size = getline(&input, &input_size, stdin);
		if(strcmp(input, "exit\n")==0){
			break;
		}
		int num_pipes = makeargv(input, "|", &pipeArgs);

		//Use built-in functions
		if(num_pipes == 1){
			makeargv(pipeArgs[0],"\t \n&", &argPip);
			lenArg = strlen(pipeArgs[0]);
			if(strcmp(argPip[0],"cd")==0){
				chdir(argPip[1]);
				continue;
			}
			else if(strcmp(argPip[0],"echo")==0){
				write(1, pipeArgs[0]+5,lenArg-5);
				continue;	
			}
		}



		// Create a child process to handle user input. 
		if((pid = fork()) == -1){
			write(2, "Error forking\n", 14);
		} else if(pid == 0)// Child process
		{
			int fdl[2];
			int fdr[2];
			
			// Handle user input here!			

			for (int i = num_pipes - 1; i >= 0; i--){

				//Variables
				int outputFileDescriptor, inputFileDescriptor;
				char **argPip;
				char **clearedRedirection;
				char **redirectionIn;
				char **redirectionOut;


				makeargv(pipeArgs[i], "\t \n&", &argPip);

				//If we are at the end of commands
				if(i==0){

					//Check for redirection
					if(makeargv(pipeArgs[i], ">", &redirectionOut)>1){
						//Clear up the new lines
						makeargv(redirectionOut[1], " \n\t", &clearedRedirection);
						outputFileDescriptor = creat(clearedRedirection[0], 0777);
						if(outputFileDescriptor<0){ 
							write(2, "errrr\n", 6);
						}
						dup2(outputFileDescriptor, STDOUT_FILENO);
						//clean more
						makeargv(redirectionOut[0], "\t \n&", &argPip);
					}

					if(makeargv(redirectionOut[0], "<", &redirectionIn)>1){
						//Same as before, clean then redirect
						makeargv(redirectionIn[1], " \n\t", &clearedRedirection);
						inputFileDescriptor = open(clearedRedirection[0], O_RDONLY);
						dup2(inputFileDescriptor, STDIN_FILENO);
						//clean moar
						makeargv(redirectionIn[0], "\t \n&", &argPip);
					}
					execvp(argPip[0], &argPip[0]);
				}
				
				pipe(fdl);				
				if((pid = fork())==-1){
					write(2,"Err forking\n", 12);
				}
				
				else if(pid > 0){

					//if so, check for redirection
					if(i == num_pipes-1){

						if(makeargv(pipeArgs[i], "<", &redirectionIn)>1){
							//Same as before, clean
							makeargv(redirectionIn[1], " \n\t", &clearedRedirection);
							inputFileDescriptor = open(clearedRedirection[0], O_RDONLY);
							makeargv(redirectionIn[0], "\t \n&", &argPip);
							close(fdl[1]);
							dup2(inputFileDescriptor, STDIN_FILENO);
							//clean then exec
							execvp(argPip[0], &argPip[0]);

						}else{
							close(fdl[1]);
							dup2(fdl[0], STDIN_FILENO);
							execvp(argPip[0],&argPip[0]);
						}

					}else{
						close(fdl[1]);
						dup2(fdl[0], STDIN_FILENO);
						execvp(argPip[0], &argPip[0]);
					}

				} else if(pid == 0){

					if(i == num_pipes-1){

						if(makeargv(pipeArgs[i], ">", &redirectionOut)>1){
							makeargv(redirectionOut[1], " \n\t",&clearedRedirection);
							outputFileDescriptor = creat(clearedRedirection[0], 0777);
							//Make the output of one,the input of another
							fdr[0] = fdl[0];
							fdr[1] = outputFileDescriptor;
							dup2(outputFileDescriptor, STDOUT_FILENO);
							close(fdr[0]);
							makeargv(redirectionOut[0], "\t \n&", &argPip);
						
						}else{
							fdr[0] = fdl[0];
							fdr[1] = fdl[1];
							dup2(fdr[1], STDOUT_FILENO);
							close(fdr[0]);
						}

					} else {
						fdr[0] = fdl[0];
						fdr[1] = fdl[1];
						dup2(fdr[1], STDOUT_FILENO);
						close(fdr[0]);
					}

				}



			}

		free(input);
		//.... boom
		exit(0);

		} else if(pid != 0) {
			wait(NULL);
		}
	}
}
