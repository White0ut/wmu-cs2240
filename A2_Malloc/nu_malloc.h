// Included for the `size_t` type. 
#include <string.h>
#include <unistd.h>



#define NALLOC 1024

typedef long Align; 		/* for alignment to long boundary */
union header {
	struct {
		union header *ptr; 	/*next block if on free list */
		size_t size; 		/* size of this block */
	} s;
	Align x;
};
typedef union header Header;
void* nu_malloc(size_t);
void* nu_calloc(size_t, size_t);
void* nu_realloc(void*, size_t);
static Header* nu_memget(size_t);
void nu_free(void*);
