#include "main.h"


/*
* Sort function for use with qsort
* returns the difference of a and b's database index
*/
int compf(const void *a, const void *b) {
	DataStruct *argA;
	DataStruct *argB;

	argA = (DataStruct *) a;
	argB = (DataStruct *) b;

	return argA->database_idx - argB->database_idx;
	
}


void *threadSort(void *file_struct) {
	// Variables
	size_t len = 0;
	int i, j;
	char *savePtr, *token;
	char line[1024];
	FileStruct *data_struct = (FileStruct *) file_struct;
	data_struct->member_count = 0;
	FILE *file = fopen(data_struct->file_name, "r");

	// Loop while there is still user data
	i=0;
	while ((fgets(line, 1024, file))) {

		data_struct->member_count++;
		strncpy(data_struct->user_array[i].data, line, strlen(line));

		// Split string on "," then save the index
		token = strtok_r(line, ",", &savePtr);
		for(j=0; j<4; j++){
			token = strtok_r(NULL, ",", &savePtr);
			if(j == 3) {
				data_struct->user_array[i].database_idx = atoi(token);
			}
		}

	i++;
	}

	// Sort with the compf function defined above
	qsort(data_struct->user_array, data_struct->member_count, 
							sizeof(*data_struct->user_array), compf);

	pthread_exit(NULL);

}

/*
* This is the main function of a proof of concept for a Parallel Sort
* it accepts one argument from command line containing a directory with the data files
* files must follow this regex
*		<username>,<password><blood type>,<domain name>,<database index>
*
* It will spin up a thread for each file
* On completion it will 'merge' them and print them out to
			"sorted.yay"
*
*/
int main(int argc, char *argv[]) {

	// Variables
	DIR *d;
	struct dirent *e;
	int file_count = 0, i;
	char file_names[1024][1024];

	


	if(argc != 2) {
		//write(2, "Error, must supply a directory\n", 31);
		printf("Error, must supply a directory\n");
		return 0;
	}


	d = opendir(argv[1]);

	// Loop and get and file_count and an array of filenames
	while((e = readdir(d)) != NULL) {
		if(strncmp(e->d_name, ".", 1) == 0)
			continue;
		else if(strncmp(e->d_name, "..", 2) == 0)
			continue;
		strncpy(file_names[file_count], e->d_name, strlen(e->d_name));
		file_count++;
	}

	// Set directory to the directory with data
	chdir(argv[1]);

	FileStruct thread_array[file_count];


	// Initiate file_names and spin up threads
	for(i=0; i<file_count; i++) {

		// Set filenames of the Threads
		thread_array[i].file_name = (char*)malloc(strlen(file_names[i]));
		strncpy(thread_array[i].file_name, file_names[i],strlen(file_names[i]));

		// Create the Thread 
		pthread_create(&thread_array[i].thread_ptr, NULL, &threadSort, &thread_array[i]);
	}

	for(i=0; i<file_count; i++)
		pthread_join(thread_array[i].thread_ptr, NULL);




	// Variables -- round 2
	// Back up the directory
	chdir("..");
	FILE *f_done = fopen("sorted.yay", "w");
	int lowest = 0, data_left = 1, idx[file_count], lowest_idx = 0;
	char *lowest_line = (char *) malloc(sizeof(char *));
	for(i=0; i<file_count; i++) {
		idx[i] = 0;
	}

	// Merge step of a merge sort
	// AKA: grab the lowest data from the top of our array of sorted user arrays
	while(data_left) {

		data_left = 0;
		lowest = 2000000000;

		for(i=0; i<file_count; i++) {
			if(lowest > thread_array[i].user_array[idx[i]].database_idx && 
					idx[i] <= thread_array[i].member_count) {

				lowest = thread_array[i].user_array[idx[i]].database_idx;
				lowest_idx = i;
				data_left = 1;
				
			}
		}
		fprintf(f_done, "%s", thread_array[lowest_idx].user_array[idx[lowest_idx]].data);
		idx[lowest_idx]++;


	}


	
	// Free memory
	for(i=0; i<file_count; i++) {
		free(thread_array[i].file_name);
	}
	fclose(f_done);
	free(lowest_line);
}