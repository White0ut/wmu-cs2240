#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>


typedef struct DataStruct {
	char data[75];
	int database_idx;
} DataStruct;

typedef struct FileStruct {
	char *file_name;
	int member_count;
	DataStruct user_array[500];
	pthread_t thread_ptr;
} FileStruct;

int compf(const void *a, const void *b);
int main(int argc, char *argv[]);
void *threadSort(void *f);