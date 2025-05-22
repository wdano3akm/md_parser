#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "writer.h"

int writer(char* file_content, char file_name[]){
	FILE *fp;


	fp = fopen(file_name, "w");
	fwrite(file_content, 1, strlen(file_content), fp);
	return 0;
}
