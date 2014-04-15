#include "map.h"


void map_init(map_t* self) {
	self->size=0;
	self->entry = NULL;
}

int map_put(map_t* self, const char* key, const char* val) {

	// Assertions
	assert(self != NULL);

	//Check for dup
	if(map_get(self,key)!=NULL){
		return KEY_EXISTS;
	}

	// Variable declarations
	map_entry_t *current;
	const size_t len_key = strlen(key)+1;
	const size_t len_val = strlen(val)+1;

	// Sets up insertion Node
	current = (map_entry_t*)(malloc(sizeof(map_entry_t)));
	current->key = (char*)(malloc(sizeof(char*)));
	current->value = (char*)(malloc(sizeof(char*)));
	strncpy(current->key,key,len_key);
 	strncpy(current->value,val,len_val);

 	// Inserts in front
 	current->next = self->entry;
 	self->entry = current;

 	// Increment size counter at end
 	self->size++;

 	return OK;
}

const char* map_get(map_t* self, const char* key) {
	// Assertions
	assert(self != NULL);

	// Variable declarations
	const size_t len = strlen(key)+1;
	char *comp = (char *)malloc(len);
	strncpy(comp,key,len);
	map_entry_t *iterator;
	iterator=self->entry;

	if(map_size(self)==0) {
		return NULL;
	}


	else if(iterator->next==NULL) {
		return NULL;
	}

	while(strncmp(iterator->key,comp,len) != 0) {
		if(iterator->next==NULL) {
			return NULL;
		} else {
			iterator = iterator->next;
		}
	}
	return iterator->value;

}

int map_size(map_t* self) {
	// Assertions
	assert(self != NULL);

	return self->size;  
}

int map_remove(map_t* self, const char* key) {
	// Assertions
	assert(self != NULL);

	// Variable declarations
	char *comp = (char*)malloc(sizeof(char*));
	strcpy(comp,key);
	map_entry_t *iterator, *prev;
	iterator=self->entry;

	// Head node is the one specified
	if(strcmp(iterator->key, comp)==0) {
		self->entry = iterator->next;
		self->size--;
		return OK;
	}

	if(iterator->next==NULL) {
		return NO_KEY_EXISTS;
	}
	while(strcmp(iterator->key,comp) != 0) {
		if(iterator->next==NULL) {
			return NO_KEY_EXISTS;
		}else {
			prev=iterator;
			iterator = iterator->next;
		}
	}
		prev->next = iterator->next;
	
	self->size--;
	return OK;

}

int map_serialize(map_t* self, FILE* stream) {
	// Assertions
	assert(self != NULL);

	char* str = (char*)malloc(sizeof(char*));
	map_entry_t *p, *q;

	// Write to file

	for(p = self->entry; p != NULL; p = q) {

		q = p->next;

		strcpy(str,p->key);
		strcat(str,":");
		strcat(str,p->value);
		strcat(str,"\n");
		// Write to file
		fwrite(str, 1,strlen(str), stream);
		//printf("wrote %s",str);

	}
	// Close file


	return OK;

}

int map_deserialize(map_t* self, FILE* stream) {
	// Assertions
	assert(self != NULL);

	// Variable declarations
	int BUFFER_SIZE = 1024;
	char buffer[BUFFER_SIZE];
	int n_reads = 0, char_count=0, char_read;
	char* value, *token, *key, *val;

	// Re-fills the buffer
	while(1) {
		char_read = fread(buffer, 1, BUFFER_SIZE, stream);
		//printf("hre\n"); 			// Debug purposes
		buffer[char_read] = 0;
		//printf("%d\n",char_read); // Debug purposes

		value = strndup(buffer, BUFFER_SIZE);

		// Splits up the buffer
		while((token = strsep(&value, "\n")) !=NULL && token[0] != '\0'){
			key = strsep(&token, ":");
			val = token;
			map_put(self,key,val);
		}

		if(feof(stream)){
			break;
		 }

		printf("Seek needed\n");
		fseek(stream, (BUFFER_SIZE*n_reads +strlen(value)), SEEK_SET);
		printf("%d\n",(int)(BUFFER_SIZE*n_reads +strlen(value)));
		n_reads++;
	}	
	return OK;

}

void map_destroy(map_t* self) {
	// Assertions
	assert(self != NULL);
	map_entry_t *p, *q;

	for(p = self->entry; p != NULL; p = q) {
		q = p->next;
		free(p->key);
		free(p->value);
		free(p);
		self->size--;
	}	
}
