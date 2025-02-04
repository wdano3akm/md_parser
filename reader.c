#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "parser.h"

char* read_file(char file_path[]){
	FILE *fp;	
	struct stat buffer;


	fp = fopen(file_path, "r");
	if (!fp) {
		perror("fopen");
		return NULL;
	}
	int fildes = open(file_path, O_RDWR);
	int status = fstat(fildes ,&buffer);
	char* read_chars = malloc(buffer.st_size);


	size_t ret = fread(read_chars,sizeof(*read_chars), buffer.st_size,fp);
	if (ret != buffer.st_size){
		fprintf(stderr, "oh fuck, read: %zu\nread_charser: %zu", ret, buffer.st_size);
		return NULL;
	}

	//	printf("%s",read_chars );
	return read_chars;

}
/*
int main(){
	read_file("test.txt");
	return 0;
}
*/
