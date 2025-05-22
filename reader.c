//#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int check_path(char*);

char* read_file(char* file_path){
	int fildes;
	struct stat buffer;

	fildes = check_path(file_path);
	int status = fstat(fildes ,&buffer);
	char* read_chars = malloc(buffer.st_size );

	
	size_t ret = read(fildes, read_chars, buffer.st_size );
	if (ret != buffer.st_size){
		fprintf(stderr, "oh fuck, read: %zu\nread_charser: %zu", ret, buffer.st_size);
		return NULL;
	}
	

	return read_chars;

}

int check_path(char* file_path){
	int fd;
	if (file_path[0] == '/'){
		fd = open(file_path, O_RDONLY);
	} else {
		char* tmp = malloc(strlen(file_path) + 3);
		sprintf(tmp, "./%s", file_path);
		file_path = tmp;
		fd = open(file_path, O_RDONLY);
//		printf("%s", file_path);
//		printf("%d", fd);
	};

	if (fd < 0){
		fprintf(stderr, "can't find the file\n");
		exit(1);
	}
	return fd;
}

/*
int main(int argc, char **argv){

	if (argc < 2) {
		fprintf(stderr, "please provide file\n");
		exit(1);

	}
	char* string = read_file(argv[1]);
	printf("%s", string);

	return 0;
}
*/
