#include <stddef.h>
#include <stdlib.h>
#include <string.h>
typedef struct dstirng {
	char *ptr;
	size_t size;
} dstring;

char* init(size_t);
void append(dstring*, char*);



dstring miao(size_t start_size){
	dstring out;
	if (start_size < 1) return out; 
	char* base_alloc = malloc(start_size);
	out.size = start_size;
	out.ptr = base_alloc;
	return out ;
}

void append(dstring* dstring, char* c){
	size_t len = strlen(dstring->ptr); // excludes \0 in calc
	size_t	new_len = strlen(c) + len + 1;
	if (new_len >= dstring->size) {
		dstring->ptr = realloc(dstring->ptr, dstring->size * 2);
		dstring->size = dstring->size * 2;
	}
	strcat(c, dstring->ptr);
}

