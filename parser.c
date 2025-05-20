#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STACK_SIZE 2048
#define MAXLEN 2048 * 10

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
char* parse_line_md(char*);
char* parse_block_md(char*);

/*
int main(){
	char* text = parse_inline_md("");
	int i = 0;
	while (text[i]) putchar(text[i++]);
	return 0;
}
*/

char* parse_line_md(char *md_string){
	int i = 0;
	char *html = malloc(MAXLEN);
	*html = '\0';


	for(; md_string[i] ; i++){
		switch (md_string[i]) {
			case '*': 
				int x = pop();
				if (md_string[++i] == '*'){
					if (x == BOLD){
						strcat(html, "</strong>");
					} else {
						push(x);
						push(BOLD);
						strcat(html, "<strong>");
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
				
				return html;
				free( html);
				html = NULL;
				break;
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
				if (md_string[i-1] != '\n')
					continue;
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
				html[c+1] = '\0';
		}
	}

	return html;
	free( html);
	html = NULL;
}


char* parse_block_md(char *md_string){
	int i = 0;
	char *html = malloc(MAXLEN);
	html[0] = 0;


	for(; md_string[i] ; i++){
		switch (md_string[i]) {
			case '*': 
				int x = pop();
				if (md_string[++i] == '*'){
					if (x == BOLD){
						strcat(html, "</strong>");
					} else {
						push(x);
						push(BOLD);
						strcat(html, "<strong>");
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
					i--;
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
				if ((md_string[i-1] == '\n') && md_string[i+1] == '[' && md_string[i+2] == '[') {
					int start_link = i+3;
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
				} else {
					int c = strlen(html);
					html[c] = md_string[i];
					html[c+1] = 0;
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
				html[c+1] = 0;
		}
	}

	return html;
	free( html);
	html = NULL;
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



#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define NEXTLINE(a)    a = find_next_char(a, '\n')

char input[MAXLEN];
char output[MAXLEN];

enum BLOCKTYPE {
	BT_NONE,
	BT_HEADER,
	BT_LIST,
	BT_CODE,
	BT_TEXT
};

char *
find_char(char *in, char c) {
	while (*in != c && *in != '\0')
		in++;
	return in;
}

char *
find_next_char(char *in, char c) {
	in = find_char(in, c);
	if (*in != '\0') in++;
	return in;
}

char *
find_list_start(char *in) {
	while (*in == '\t') in++;

	if (*in != '*' && *in != '-') {
		while (isdigit(*in)) in++;
	}
	in++;

	while (isspace(*in)) in++;
	return in;
}

int
is_list_item(char *in) {
	int res = 0;
	while (*in == '\t') {
		++in; ++res;
	}

	if (strncmp(in, "* ", 2) == 0 || strncmp(in, "- ", 2) == 0) return res;

	if (!isdigit(*in)) return -1;
	while (isdigit(*in)) in++;
	return (strncmp(in, ". ", 2) == 0) ? res : -1;
}

char *
start_block(char *out, enum BLOCKTYPE ctype, int hcnt, int ldepth)
{
/*	fprintf(stderr, "Opening ");*/
	switch (ctype) {
		case BT_HEADER:
/*			fprintf(stderr, "HEADER");*/
			strcat(out, "<h0>");
			out[strlen(out)-2] += hcnt;
			break;
		case BT_LIST:
/*			fprintf(stderr, "LIST");*/
			out = find_char(out, '\0');
			for (; ldepth; out++, ldepth--)
				out[0] = '\t';
			out[0] = '\0';
			strcat(out, "<ul>\n");
			break;
		case BT_CODE:
/*			fprintf(stderr, "CODE");*/
			strcat(out, "<code>\n");
			break;
		case BT_TEXT:
/*			fprintf(stderr, "TEXT");*/
			strcat(out, "<p>\n");
			break;
	}
/*	fprintf(stderr, "\n");*/

	return find_char(out, '\0');
}

char *
add_list_item(char *out, int ldepth, char *start, char *end)
{
	char temp;
	for (ldepth++ ; ldepth; ldepth--, out++)
		out[0] = '\t';
	out[0] = '\0';
	strcat(out, "<li>");

	temp = *end;
	*end = '\0';
	// check char* start
	/* TODO:, actually process */
	start = parse_line_md(start);
	strcat(out,start);
	*end = temp;
	strcat(out, "</li>\n");

	return find_char(out, '\0');
}

char *
end_block(char *out, enum BLOCKTYPE ctype, int hcnt, int ldepth, char *start, char *end)
{
	char temp;
/*	fprintf(stderr, "Closed ");*/
	switch (ctype) {
		case BT_HEADER:
/*			fprintf(stderr, "HEADER");*/
			temp = *end;
			*end = '\0';
			
			strcat(out, start);
			*end = temp;
			strcat(out, "</h0>\n");
			out[strlen(out)-3] += hcnt;
			break;
		case BT_LIST:
/*			fprintf(stderr, "LIST");*/
			out = find_char(out, '\0');
			for (; ldepth; out++, ldepth--)
				out[0] = '\t';
			out[0] = '\0';
			strcat(out, "</ul>\n");
			break;
		case BT_CODE:
/*			fprintf(stderr, "CODE");*/
			temp = *end;
			*end = '\0';
			strcat(out, start);
			*end = temp;
			strcat(out, "\n</code>\n");
			break;
		case BT_TEXT:
/*			fprintf(stderr, "TEXT");*/
			temp = *end;
			*end = '\0';
			/* TODO:, actually process */
			start = parse_block_md(start);
			strcat(out, start);
			*end = temp;
			strcat(out, "\n</p>\n");
			break;
	}
/*	fprintf(stderr, "\n");*/

	return find_char(out, '\0');
}

char *
parse(char *in)
{
	int hcnt, i, j, k, ldepth;
	char *start, *end, *pnt;
	char *out = output;
	ldepth = -1;
	out[0] = '\0';
	pnt = out;
	enum BLOCKTYPE ctype = BT_NONE;

	while (*in != '\0') {
		/* Code blocks have different rules */
		if (ctype == BT_CODE) {
			if (strncmp(in, "```", 3) == 0) {
				end = in-1;
				pnt = end_block(pnt, ctype, hcnt, ldepth, start, end);
				in += 3;
				ctype = BT_NONE;
				ldepth = -1;
			} else {
				NEXTLINE(in);
			}
			continue;
		} else {
			/* HEADER START */
			if (*in == '#') {
				while (ldepth != -1)
					pnt = end_block(pnt, ctype, hcnt, ldepth--, start, in-1);
				ldepth = -1;
				ctype = BT_NONE;
				hcnt = 0;
				while (*in == '#') {
					in++;
					hcnt++;
				}

				/* Remove white spaces */
				while (isspace(*in)) in++;

				hcnt = hcnt > 6 ? 6 : hcnt;

				start = in;
				end = find_char(start, '\n');
				pnt = start_block(pnt, BT_HEADER, hcnt, ldepth);
				pnt = end_block(pnt, BT_HEADER, hcnt, ldepth, start, end);
				NEXTLINE(in);
				continue;
			}

			/* CODE BLOCK START */
			if (strncmp(in, "```", 3) == 0) {
				while (ldepth != -1)
					pnt = end_block(pnt, ctype, hcnt, ldepth--, start, in-1);
				ldepth = 0;
				NEXTLINE(in);
				start = in; /* Probably Undefined behaviour if ```\n``` */
				ctype = BT_CODE;
				pnt = start_block(pnt, ctype, hcnt, ldepth);
				continue;
			}

			i = is_list_item(in);
			if (i != -1) {
				if (ctype != BT_LIST) {
					pnt = end_block(pnt, ctype, hcnt, ldepth--, start, in-1);
					ldepth = -1;
					ctype = BT_LIST;
				}

				while (ldepth > i)
					pnt = end_block(pnt, ctype, hcnt, ldepth--, start, end);
				while (ldepth < i)
					pnt = start_block(pnt, ctype, hcnt, ++ldepth);

				start = find_list_start(in);
				end = find_char(start, '\n');
				pnt = add_list_item(pnt, ldepth, start, end);
				NEXTLINE(in);
				ldepth = i;
				continue;
			}

			/* Blank line stops everything */
			if (*in == '\n') {
				while (ldepth != -1)
					pnt = end_block(pnt, ctype, hcnt, ldepth--, start, in-1);
				ldepth = -1;
				ctype = BT_NONE;
			} else if (ctype != BT_TEXT) {
				while (ldepth != -1)
					pnt = end_block(pnt, ctype, hcnt, ldepth--, start, in-1);
				ldepth = 0;
				ctype = BT_TEXT;
				pnt = start_block(pnt, ctype, hcnt, ldepth);
				start = in;
			}
			NEXTLINE(in);
		}
	}

	while (ldepth != -1)
		pnt = end_block(pnt, ctype, hcnt, ldepth--, start, in-1);

	return out;
}


int
main(int argc, char **argv)
{
	FILE *mdin = fopen(argv[1], "r");
	fread(input, 1, MAXLEN, mdin);
	fclose(mdin);

	parse(input);
	fprintf(stdout, "%s", output);
}
