#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dstring.h"
#include "config.h"

#define STACK_SIZE 2048
#define MAXLEN 2048

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
void parse_line_md(dstring *, char*);
void parse_block_md(dstring *, char*);
void add_file(dstring *, char*);

void
parse_line_md(dstring *html, char *md_string)
{
    fprintf(stderr, "PARSE LINE:\n%s\n", md_string);
	int i = 0;
	char *tail;

	for(; md_string[i]; i++){
		switch (md_string[i]) {
			case '*':
				int x = pop();
				if (md_string[++i] == '*'){
					if (x == BOLD){
						append(html, "</strong>");
					} else {
						push(x);
						push(BOLD);
						append(html, "<strong>");
					}
				}else{
					i--;
					if (x == ITALIC){
						append(html, "</em>");
					} else {
						push(x);
						push(ITALIC);
						append(html, "<em>");
					}
				}
				break;
			case '\n':
//				fprintf(stderr, "%s:%d: Shouldn't reach this code\n", __FILE__, __LINE__);
				return;
			case '`':
				x = pop();
				if (x == CODE){
					append(html, "</code>");
				} else {
					push(x);
					push(CODE);
					append(html, "<code>");
				} break;
			case '!':
				if (i != 0 && md_string[i-1] != '\n')
					continue;
				tail = html->tail;
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

							append(html, buffer);

							free(buffer);
							break;
						}
					}
					if (tail == html->tail) {
						i = start_link;
						append(html, "![[");
					}
				}
				break;
			case '[':
				int start_alias = ++i;
				tail = html->tail;
				while (md_string[++i]) {
					if (strncmp(md_string+i, "](", 2)) {
                        i += 2;
						char *start_link = md_string + i;
						while (md_string[++i]) {
							if (md_string[i] == ')') {
                                start_link[-2] = 0;
                                md_string[i] = 0;

                                dsprintf(html, "<a href=\"%s\">%s</a>", start_link, md_string + start_alias);


                                start_link[-2] = ']';
                                md_string[i] = ')';
                                break;
							}
						}
						break;
					}
				}
				if (tail == html->tail) {
					i = start_alias;
					append(html, "[");
				}
				break;
			default:
				append_c(html, md_string[i]);
		}
	}
}


void
parse_block_md(dstring *html, char *md_string){
	int i = 0;
	char *tail;
    fprintf(stderr, "PARSE BLOCK:\n%s\n", md_string);

	for(; md_string[i]; i++){
		switch (md_string[i]) {
			case '*':
				int x = pop();
				if (md_string[++i] == '*'){
					if (x == BOLD){
						append(html, "</strong>");
					} else {
						push(x);
						push(BOLD);
						append(html, "<strong>");
					}
				}else{
					i--;
					if (x == ITALIC){
						append(html, "</em>");
					} else {
						push(x);
						push(ITALIC);
						append(html, "<em>");
					}
				}
				break;
			case '\n':
				// check for 2 consecutive \n to </p> <p> or just one to \br
				if (md_string[++i] == '\n'){
					append(html, "</p>\n<p>\n");
//					fprintf(stderr, "%s:%d: Shouldn't reach this code\n", __FILE__, __LINE__);
				} else {
					append(html, "<br />\n");
					i--;
				} break;
			case '`':
				x = pop();
				if (x == CODE){
					append(html, "</code>");
				} else {
					push(x);
					push(CODE);
					append(html, "<code>");
				} break;
			case '!':
				tail = html->tail;
				if ((i == 0 || md_string[i-1] == '\n') && md_string[i+1] == '[' && md_string[i+2] == '[') {
					int start_link = i+3;
					while (md_string[++i]) {
						if (!strncmp(&md_string[i], "]]", 2)) {
							i += 1;
							int length = i - start_link - 1;

							char *file_location = &md_string[start_link];
							file_location[length] = '\0';
							
							add_file(html, file_location);

							file_location[length] = ']';
							break;
						}
					}
				
					if (tail == html->tail) {
						i = start_link;
						append(html, "![[");
					}
					
				} else {
					append_c(html, md_string[i]);
				}
				break;
			case '[':
				int start_alias = ++i;
				tail = html->tail;
				while (md_string[++i]) {
					if (!strncmp(md_string+i, "](", 2)) {
                        i += 2;
						char *start_link = md_string + i;
						while (md_string[++i]) {
							if (md_string[i] == ')') {
                                start_link[-2] = 0;
                                md_string[i] = 0;

                                dsprintf(html, "<a href=\"%s\">%s</a>", start_link, md_string + start_alias);


                                start_link[-2] = ']';
                                md_string[i] = ')';
                                break;
							}
						}
						break;
					}
				}
				if (tail == html->tail) {
					i = start_alias;
					append(html, "[");
				}
				break;
			default:
				append_c(html, md_string[i]);
		}
	}
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

void
start_block(dstring *out, enum BLOCKTYPE ctype, int hcnt, int ldepth)
{
/*	fprintf(stderr, "Opening ");*/
	switch (ctype) {
		case BT_HEADER:
/*			fprintf(stderr, "HEADER");*/
			append(out, "<h0>");
			out->tail[-2] += hcnt;
			break;
		case BT_LIST:
/*			fprintf(stderr, "LIST");*/
			for (; ldepth; ldepth--)
				append(out, "\t");
			append(out, "<ul>\n");
			break;
		case BT_CODE:
/*			fprintf(stderr, "CODE");*/
			append(out, "<code>\n");
			break;
		case BT_TEXT:
/*			fprintf(stderr, "TEXT");*/
			append(out, "<p>\n");
			break;
	}
/*	fprintf(stderr, "\n");*/
}

void
add_list_item(dstring *out, int ldepth, char *start, char *end)
{
	char temp;
	for (ldepth++ ; ldepth; ldepth--)
		append(out, "\t");
	append(out, "<li>");

	temp = *end;
	*end = '\0';
	// check char* start
	/* TODO:, actually process */
	parse_line_md(out, start);
	*end = temp;
	append(out, "</li>\n");
}

void
end_block(dstring *out, enum BLOCKTYPE ctype, int hcnt, int ldepth, char *start, char *end)
{
	char temp;
/*	fprintf(stderr, "Closed ");*/
	switch (ctype) {
		case BT_HEADER:
/*			fprintf(stderr, "HEADER");*/
			temp = *end;
			*end = '\0';
			
			append(out, start);
			*end = temp;
			append(out, "</h0>\n");
			out->tail[-3] += hcnt;
			break;
		case BT_LIST:
/*			fprintf(stderr, "LIST");*/
			for (; ldepth; ldepth--)
				append(out, "\t");
			append(out, "</ul>\n");
			break;
		case BT_CODE:
/*			fprintf(stderr, "CODE");*/
			temp = *end;
			*end = '\0';
			append(out, start);
			*end = temp;
			append(out, "\n</code>\n");
			break;
		case BT_TEXT:
/*			fprintf(stderr, "TEXT");*/
			temp = *end;
			*end = '\0';
			/* TODO:, actually process */
			parse_block_md(out, start);
			*end = temp;
			append(out, "\n</p>\n");
			break;
	}
/*	fprintf(stderr, "\n");*/
}

char *
parse(char *in)
{
	int hcnt, i, j, k, ldepth;
	char *start, *end;
	dstring dout;
	ldepth = -1;
	dout = init(MAXLEN);
	enum BLOCKTYPE ctype = BT_NONE;

	while (*in != '\0') {
		/* Code blocks have different rules */
		if (ctype == BT_CODE) {
			if (strncmp(in, "```", 3) == 0) {
				end = in-1;
				end_block(&dout, ctype, hcnt, ldepth, start, end);
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
					end_block(&dout, ctype, hcnt, ldepth--, start, in-1);
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
				start_block(&dout, BT_HEADER, hcnt, ldepth);
				end_block(&dout, BT_HEADER, hcnt, ldepth, start, end);
				NEXTLINE(in);
				continue;
			}

			/* CODE BLOCK START */
			if (strncmp(in, "```", 3) == 0) {
				while (ldepth != -1)
					end_block(&dout, ctype, hcnt, ldepth--, start, in-1);
				ldepth = 0;
				NEXTLINE(in);
				start = in; /* Probably Undefined behaviour if ```\n``` */
				ctype = BT_CODE;
				start_block(&dout, ctype, hcnt, ldepth);
				continue;
			}

			i = is_list_item(in);
			if (i != -1) {
				if (ctype != BT_LIST) {
					end_block(&dout, ctype, hcnt, ldepth--, start, in-1);
					ldepth = -1;
					ctype = BT_LIST;
				}

				while (ldepth > i)
					end_block(&dout, ctype, hcnt, ldepth--, start, end);
				while (ldepth < i)
					start_block(&dout, ctype, hcnt, ++ldepth);

				start = find_list_start(in);
				end = find_char(start, '\n');
				add_list_item(&dout, ldepth, start, end);
				NEXTLINE(in);
				ldepth = i;
				continue;
			}

			/* Blank line stops everything */
			if (*in == '\n') {
				while (ldepth != -1)
					end_block(&dout, ctype, hcnt, ldepth--, start, in-1);
				ldepth = -1;
				ctype = BT_NONE;
			} else if (ctype != BT_TEXT) {
				while (ldepth != -1)
					end_block(&dout, ctype, hcnt, ldepth--, start, in-1);
				ldepth = 0;
				ctype = BT_TEXT;
				start_block(&dout, ctype, hcnt, ldepth);
				start = in;
			}
			NEXTLINE(in);
		}
	}

	while (ldepth != -1)
		end_block(&dout, ctype, hcnt, ldepth--, start, in-1);

	return dout.ptr;
}

void add_file(dstring *str, char *filepath){
	char *ext;
	char *end = strrchr(filepath, '.');
	if (!end){
		// TODO: think about edge cases
		dsprintf(str,"<%s src=\"%s%s\"></%s>",  ext , prefix_path,  filepath, ext);
		return;
	}
	end++;
	switch (*end) {
		case 'p': if (*++end== 'd') ext = "iframe"; else ext = "img"; break;
		case 'j': ext = "img"; break;
		default: ext = "iframe"; break;
	}
	dsprintf(str,"<%s src=\"%s%s\"></%s>",  ext , prefix_path,  filepath, ext);
}
