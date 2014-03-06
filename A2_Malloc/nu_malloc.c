#include "nu_malloc.h"


static Header base;				/* empty list to get started */
static Header *freep;			/* start of the free list */

void* nu_malloc(size_t size) {

	Header *p, *prevp;
	size_t nunits;

	nunits = (size+sizeof(Header)-1)/sizeof(Header)+1; 

	if((prevp = freep) == NULL) {
		base.s.ptr = freep = prevp = &base;
		base.s.size = 0;
	}
	for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) {
		if (p->s.size >= nunits) {		/* big enough */
			if (p->s.size == nunits) {	/* exact fit */
				prevp->s.ptr = p->s.ptr;
			} else {					/* allocate tail end */
				p->s.size -= nunits;
				p += p->s.size;
				p->s.size = nunits;
			}
			freep = prevp;
			return (void *)(p+1);
		}
		if(p == freep) {					/* wrapped around free list */
			if((p=nu_memget(nunits)) == NULL)  {
				return NULL;
			}
		}
	}
}
static Header* nu_memget(size_t size) {
		char *cp;
		Header *up;

		if(size < NALLOC) {
			size = NALLOC;
		}
		cp = (char*)sbrk((size * sizeof(Header)));
		if (cp == (char*) - 1){ /* no space at all*/
			return NULL;
		}
		up =(Header*) cp;
		up->s.size = size;
		nu_free((void*)(up+1));
		return freep;
}

void* nu_calloc(size_t count, size_t size) {

	size_t  i;
	char* bar;
	Header* p;

	if(count == 0 || size == 0){
		return NULL;
	}
	p = (Header*)nu_malloc(size*count);
	
	// Zero out the buffer 
	// bar = (char*)p + size - size%sizeof(size_t);
	// for(i=0; i<size%sizeof(p); i++){
	// 	bar[i] = 0;
	// }

	memset(p, 0, sizeof(p));

	return (void *)(p); 
}

void* nu_realloc(void* ptr, size_t size) {

       size_t min_size;
       void *new_p;

       if(ptr == NULL){
       	return nu_malloc(size);
       }
       
       new_p = nu_malloc(size);	

       if(new_p == NULL)
       		return NULL;

       min_size = sizeof(ptr);
       if(size < min_size)
       		min_size = size;
       memcpy(new_p, ptr, min_size);

       nu_free(ptr);

  return new_p; 
}

void nu_free(void* ap) {

	Header *bp, *p;
	bp = (Header *)ap -1;	/* point to block header */
	for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr){
		if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
			break;	/* freed block at start of endof arena */
	}

	if (bp + bp->s.size == p->s.ptr) {	/* join to upper nbr */
		bp->s.size += p->s.ptr->s.size;
		bp->s.ptr = p->s.ptr->s.ptr;
	} else 
		bp->s.ptr = p->s.ptr;
	if (p + p->s.size == bp) {			/* join to lower nbr */
		p->s.size += bp->s.size;
		p->s.ptr = bp->s.ptr;
	} else 
		p->s.ptr = bp;
	freep = p;
  
}
