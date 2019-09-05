#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#ifdef LIBRARY
#include <string.h>

#define OUTPUT(statement) (output_buffer = append_to_string(output_buffer, statement))
#define OUTPUT_NOTAB(statement) OUTPUT(statement)
#else
#define OUTPUT(statement) printf("%s%s\n", loop, statement)
#define OUTPUT_NOTAB(statement) printf("%s\n", statement)
#endif

#ifdef LIBRARY
static char* allocate_memory(char*, size_t);
static char* append_to_string(char*, char*);
       char* compile_bf(FILE*);

static inline char* append_to_string(char* string, char* append) {
	/* add one for the newline */
	string = allocate_memory(string, strlen(string) + strlen(append) + 1);
	strcat(string, append);
	strcat(string, "\n");
	return string;
}

static inline char* allocate_memory(char* memory, size_t size) {
	char* newmem = realloc(memory, size);
	if (!newmem) abort();
	return newmem;
}

char* compile_bf(FILE* given) {
	FILE* input = given;
	char* output_buffer = malloc(1);
	*output_buffer = 0;
#else
int main(int argc, char** argv) {
	FILE* input = stdin;

	if (argc > 1) input = fopen(argv[1], "r");
	if (input == NULL) {
		fprintf(stderr, "%s: error: %m\n", argv[0]);
		return 50 + (errno % 20);
	}

	/* stores tab characters */
	char loop[8];
	for (int i = 1; i < 8; i++) loop[i] = 0;
	loop[0] = '\t';
#endif

	char current;
	OUTPUT_NOTAB("/* generated by bran */");
	OUTPUT_NOTAB("#include <stdio.h>");
	OUTPUT_NOTAB("");
	OUTPUT_NOTAB("char buffer[1024];");
	OUTPUT_NOTAB("");
	OUTPUT_NOTAB("int main(void)");
	OUTPUT_NOTAB("{");
	OUTPUT("register unsigned int offset = 0;");

	while ((current = fgetc(input)) != EOF) {
		switch (current) {
			case '>':
				OUTPUT("offset++;");
				break;
			case '<':
				OUTPUT("offset--;");
				break;
			case '+':
				OUTPUT("buffer[offset]++;");
				break;
			case '-':
				OUTPUT("buffer[offset]--;");
				break;
			case '.':
				OUTPUT("printf(\"\%c\", buffer[offset]);");
				break;
			case ',':
				OUTPUT("buffer[offset] = getchar();");
				break;
			case '[':
				OUTPUT("while (buffer[offset] != 0)");
				OUTPUT("{");
#ifndef LIBRARY
				for (int i = 0; i < 8; i++)
					if (loop[i] != '\t' && (loop[i] = '\t'))
						break;
#endif
				break;
			case ']':
#ifndef LIBRARY
				for (int i = 7; i --> 0;)
					if (loop[i] == '\t' && (loop[i] = 0) == 0)
						break;
#endif
				OUTPUT("} /* end while */");
				break;
		}
	}

	OUTPUT_NOTAB("\treturn 0;");
	OUTPUT_NOTAB("}");
	OUTPUT_NOTAB("/* eof */");

#ifdef LIBRARY
	return output_buffer;
#else
	return EXIT_SUCCESS;
#endif
}
