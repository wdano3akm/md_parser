#include <stdlib.h>
#include <stdio.h>
#include "reader.h"
#include "parser.h"
#include "writer.h"

int
main(int argc, char** argv)
{
	if (argc < 2){
		fprintf(stderr, "what am i supposed to parse if you don't input a file????\n");
		exit(1);
	} else if (argc > 3) {
		fprintf(stderr, "Too many arguments used\n");
		exit(1);
	}

	char *file_contents = read_file(argv[1]);
	char *output = parse(file_contents);
	if (argc == 2)
		printf("%s", output);
	else
		writer(output, argv[2]);
	return 0;
}
