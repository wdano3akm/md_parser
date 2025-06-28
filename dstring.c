#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "dstring.h"

dstring
init(size_t size)
{
	dstring out;
	if (size < 1)
		return out;
	out.size = size;
	out.ptr = malloc(size);
	out.tail = out.ptr;
	*(out.tail) = '\0';
	return out;
}

void
append(dstring *str, char *c)
{
	size_t len, newlen;
	len = (size_t) str.tail - (size_t) str.ptr;
	newlen = strlen(c) + len + 1;
	if (new_len >= str->size) {
		str->ptr = realloc(str->ptr, str->size * 2);
		str->tail = str->ptr + len;
		str->size = str->size * 2;
	}
	strcat(str->ptr, c);
}

