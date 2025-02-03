#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "writer.h"

int writer(char* file_content, char file_name[]){
	FILE *fp;

	int len_name_arr = strlen(file_name) + 1; // len(name.md + \0)
	
	char* new_name = malloc(len_name_arr + 2);
	strncpy(new_name, file_name, len_name_arr - 3);
	strcat(new_name, "html");

	fp = fopen(new_name, "w");
	fwrite(file_content, 1, strlen(file_content), fp);
	return 0;
}
