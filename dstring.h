typedef struct dstirng {
	char *ptr, *tail;
	size_t size;
} dstring;

dstring init(size_t);
void append(dstring *, char *);
