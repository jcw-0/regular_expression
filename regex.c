#include <stdio.h>
#include <stdlib.h>
#include "regex.h"


void parse_expression(char* expr, int* instructiondest);
void reader(int* tokens);
void throw_err(char* errormsg);


int main(int argc, char** argv) {

	int* readinstructions = malloc(sizeof (int) * 256);

	parse_expression(*(++argv), readinstructions);
	reader(readinstructions);

	return 0;
}


void parse_expression(char* expr, int* instructiondest) {

	int i = 0;
	for (; *expr != '\0'; *expr++) {
		switch (*expr) {

			/* special characters / character groups */
			case '\\':
				switch (*(++expr)) {
					case 's':
						*(instructiondest + i) = WHITESPACE; i++;
					break;
					case 't':
						*(instructiondest + i) = TAB; i++;
					break;
					case 'n':
						*(instructiondest + i) = NEWLINE; i++;
					break;
					case ' ':
						*(instructiondest + i) = SPACE; i++;
					break;
					case '0':
						*(instructiondest + i) = EOFILE; i++;
					break;

					default:
					break;
				}

			break;

			/* grouping */
			case '(':
				
				/* capturing variables: '(: ...)' */
				if (*(expr + 1) == ':') {
					*(instructiondest + i) = CAPTURE; i++;
				}
				/* function mode: '(?...)' */
				else if (*(expr + 1) == '?') {
					switch (*(expr += 2)) {

						/* lookbehind */
						case '<':
							*(instructiondest + i) = IF; i++;
							*(instructiondest + i) = LOOKBEHIND; i++;
						break;

						/* lookahead */
						case '>':
							*(instructiondest + i) = IF; i++;
							*(instructiondest + i) = LOOKAHEAD; i++;
						break;

						default:
							*(instructiondest + i) = IF; i++;
						break;
					}
				break;
				}
			*(instructiondest + i) = GROUP; i++; 
			break;

			/* character sets */
			case '[':
				*(instructiondest + i) = CHARSET; i++;
			break;

			case '.':
				*(instructiondest + i) = ANY; i++;
			break;

			case '|':
				*(instructiondest + i) = OR; i++;
			break;
			
			case '!':
				*(instructiondest + i) = NOT; i++;
			break;

			default:
				*(instructiondest + i) = DEFAULT; i++;
			break;
		}

		long int q = 0;
		switch (*(expr + 1)) {
			case '*':
				*(instructiondest + i) = NOT; i++;
				*(instructiondest + i) = OR; i++;
				*(instructiondest + i) = UNTIL; i++;
			case '?':
				*(instructiondest + i) = NOT; i++;
				*(instructiondest + i) = OR; i++;
				*(instructiondest + i) = REPEAT; i++;
				*(instructiondest + i) = 1; i++;
			case '+':
				*(instructiondest + i) = REPEAT; i++;
				*(instructiondest + i) = 1; i++;
				*(instructiondest + i) = UNTIL; i++;
			case '{':
				*(instructiondest + i) = REPEAT; i++;
				q = strtol(++expr, &expr, 10);

				if (!(*expr == '}'))
					throw_err("{n} quantifier error");
				i++;
			break;
		}
	}
	*(instructiondest + i) = END;
}

void reader(int* tokens) {

	int i = 0;
	while (*tokens != END)
		switch (*tokens + (i++)) {
			case UNTIL:
				printf("UNTIL\n");
			break;
			case REPEAT:
				printf("q: %d\n", *((tokens + (++i)))); i++;		
			break;
			case NEWLINE:
				printf("NEWLINE\n");
			break;
			default:
				printf("TOKEN: %s\n", *tokens);
			break;
		}
	return;
}

void throw_err(char* errormsg) {
	printf("%s\n", errormsg);
	return;
}
