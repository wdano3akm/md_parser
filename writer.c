#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "writer.h"

int writer(char* file_content, char file_name[]) {
    FILE *fp = fopen(file_name, "w");
    if (!fp) {
        perror("fopen");
        return 1;
    }
    size_t len = strlen(file_content);  
    size_t written = fwrite(file_content, 1, len, fp);
    if (written != len) {
        perror("fwrite");
        fclose(fp);
        return 2;
    }

    fclose(fp);
    return 0;
}
