#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

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
	len = (size_t) str->tail - (size_t) str->ptr;
	newlen = strlen(c) + len + 1;
	if (newlen >= str->size) {
		str->ptr = realloc(str->ptr, str->size * 2);
		str->tail = str->ptr + len;
		str->size = str->size * 2;
	}
	strcat(str->tail, c);
	str->tail += strlen(c);
}

void
append_c(dstring *str, char c)
{
	size_t len, newlen;
	len = (size_t) str->tail - (size_t) str->ptr;
	if (len+1 >= str->size) {
		str->ptr = realloc(str->ptr, str->size * 2);
		str->tail = str->ptr + len;
		str->size = str->size * 2;
	}
	str->tail[0] = c;
	str->tail++;
	str->tail[0] = '\0';
}

void dsprintf(dstring *str, char *fmt, ...){

	va_list ap;
	int d;
	char c, *s;

	va_start(ap, fmt);
	while (*fmt){
		if (*fmt++ != '%') {
			append_c(str, fmt[-1]);
			continue;
		}
		switch (*fmt++) {
			case 's':
				s = va_arg(ap, char *);
				append(str, s);
				break;
		}
	}
	va_end(ap);
}

