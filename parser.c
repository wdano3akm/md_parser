#include <stdio.h>
#include <stdlib.h>

#define MAXVAL 1000

struct HEADER{
	int type;
};

struct LIST {
	int indent;	
};

enum block {NORMAL_TEXT, CODE_BLOCK, LIST};

int rsp = 0;

int* stack_init();
void push(int*, int);
int pop(int*);

char* parse_md(char *md_string){

	enum block my_state = NORMAL_TEXT;

	for(int i = 0; md_string[i] ; i++){
		putchar(md_string[i]);
	}
	return NULL;
}

int* stack_init(){
	int *stack;
	stack = malloc(MAXVAL);
	return stack;
}

void push(int* stack, int value){
	if(rsp < MAXVAL)stack[rsp++] = value;
	else fprintf(stderr, "stackOverflow");
}

int pop(int* stack){
	if (rsp){
		return stack[--rsp];
	} else {
		fprintf(stderr, "stack is empty");
		return -1; 
	}
}
