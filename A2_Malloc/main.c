#include "nu_malloc.c"
#include <stdio.h>
#include <stdlib.h>

union shoot{
struct {
		union header *ptr; 	/*next block if on free list */
		size_t size; 		/* size of this block */
		int a;
		char t;
		int w;
		double o;
		float q;
	} w;
};
int main(int argc, char *argv[]) {
	 char *str;

   /* Initial memory allocation */
   str = (char *) nu_malloc(15);
   strcpy(str, "test");
   printf("String = %s,  Address = %u\n", str, str);

   /* Reallocating memory */
   str = (char *) nu_realloc(str, 25);
   strcat(str, ".com");
   printf("String = %s,  Address = %u\n", str, str);

   nu_free(str);

}	
