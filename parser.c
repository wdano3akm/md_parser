#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STACK_SIZE 1000

/**
 * There are two types of md elements: BLOCK and INLINE
 *
 * BLOCK:
 *	normal_text
 *	code_block
 *	list ( + indentation)
 * INLINE:
 *	header ( + wheight 1-6)
 *	bold
 *	italic
 *	code
 *	link
 *	img
 *	pdf
 *
 * The inline we allow for multiple at the same time... this is implemented using a stack
 *
 * Instead blocks must be one at a time, so we just have a struct with different states
*/


enum block {NORMAL_TEXT, CODE_BLOCK, LIST};
enum line {BOLD, ITALIC, CODE};

int rsp = 0;
char stack[STACK_SIZE];
void push(int);
int pop();
char* parse_inline_md(char*);

int main(){
	char* text = parse_inline_md("[this_is_a_alias](and_this_is_the_link)");
	int i = 0;
	while (text[i]) putchar(text[i++]);
	return 0;
}

char* parse_inline_md(char *md_string){
	int i = 0;
	char *html = malloc(10000);


	strcat(html, "<p>");
	for(; md_string[i] ; i++){
		switch (md_string[i]) {
			case '*': 
				int x = pop();
				if (md_string[++i] == '*'){
					if (x == BOLD){
						strcat(html, "</bold>");
					} else {
						push(x);
						push(BOLD);
						strcat(html, "<bold>");
					}
				}else{
					i--;
					if (x == ITALIC){
						strcat(html, "</em>");
					} else {
						push(x);
						push( ITALIC);
						strcat(html, "<em>");
					}
				}
				break;
			case '\n': 
				// check for 2 consecutive \n to </p> <p> or just one to \br
				if (md_string[++i] == '\n'){
					strcat(html, "</p><p>");
				} else {
					strcat(html, "<br />");
				} break;
			case '`':
				x = pop();
				if (x == CODE){
					strcat(html, "</code>");
				} else {
					push( x); 
					push( CODE);
					strcat(html, "<code>");
				} break;
			case '!': 
				int start_len = strlen(html);
				if (md_string[++i] == '[' && md_string[++i] == '[') {
					int start_link = ++i;
					while (md_string[++i]) {
						if (md_string[i] == ']' && md_string[++i] == ']') {
							int length = i - start_link - 1;

							char file_location[length + 1];  
							strncpy(file_location, md_string + start_link, length);
							file_location[length] = '\0';  

							int buffer_size = length + strlen("<iframe src=\"\"></iframe>") + 1;
							char* buffer = calloc(1, buffer_size);

							strcat(buffer, "<iframe src=\"");
							strcat(buffer, file_location);
							strcat(buffer, "\"></iframe>");

							strcat(html, buffer);

							free(buffer);
							buffer = NULL;
						}
					}
					if (start_len == strlen(html)) {
						i = start_link;
						strcat(html, "![[");
					}
				}
				break;
			case '[': 
				int start_alias = ++i;
				start_len = strlen(html);
				while (md_string[++i]) {
					if (md_string[i] == ']' && md_string[++i] == '('){
						int start_link = ++i;
						while (md_string[++i]) {
							if (md_string[i] == ')') {
								int length_link = i - start_link ;
								char link[length_link + 1];
								strncpy(link,md_string + start_link, length_link);
								link[length_link] = '\0';
								int length_alias = start_link - start_alias - 2;
								char alias[length_alias+ 1];
								strncpy(alias ,md_string + start_alias, length_alias);
								alias[length_alias] = '\0';
								int anchor_len = length_alias + length_link + strlen("<a href=\"\"></a>") + 1;
								char anchor[anchor_len];
								sprintf(anchor, "<a href=\"%s\">%s</a>",link, alias);
								strcat(html, anchor);
							}
						}
						break;
					}	
				}
				if (start_len == strlen(html)){
					i = start_alias;
					strcat(html, "[");
				}

			default: 
				int c = strlen(html);
				html[c] = md_string[i];
		}
	}

	strcat(html, "</p>");
	return html;
}




void push(int value){
	if(rsp < STACK_SIZE)stack[rsp++] = value;
//	else fprintf(stderr, "stack_pOverflow");
}

int pop(){
	if (rsp){
		return stack[--rsp];
	} else {
//		fprintf(stderr, "stack_p is empty");
		return -1; 
	}
}
