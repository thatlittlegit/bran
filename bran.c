#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char INPUTMODE;
#define DEFAULT_MODE (INPUTMODE)0
#define SPECIAL_MODE (INPUTMODE)1
#define CHAR_MODE (INPUTMODE)2

static void output(const char* template, char loop[], ...);
static void cleararray(char* array, int length);
static void ouputboilerplate_before(void);
static int compile(FILE* input);

static void output(const char* template, char loop[], ...)
{
    va_list list;

    printf("%s", loop);
    va_start(list, loop);
    vprintf(template, list);
    va_end(list);
    printf("\n");
}

static void cleararray(char* array, int length)
{
    int i;
    for (i = 1; i <= length; i++)
        array[i] = 0;
}

static void ouputboilerplate_before(void)
{
    output("/* generated by bran */", "");
    output("#include <stdio.h>", "");
    output("", "");
    output("char buffer[1024];", "");
    output("", "");
    output("int main(void)", "");
    output("{", "");
    output("\tregister unsigned int offset = 0;", "");
    output("\tunsigned int iregisters[8];   /* integer registers */", "");
    output("\tchar* sregisters[8];          /* string registers (not null-terminated) */", "");
    output("\tunsigned char lregisters[8];  /* string length registers */", "");
}

int main(int argc, char** argv)
{
    FILE* input = stdin;

    if (argc > 1)
        input = fopen(argv[1], "r");
    if (input == NULL) {
        perror(argv[0]);
        return 10 + errno;
    }

    return compile(input);
}

int compile(FILE* input)
{
    char loop[255]; /* stores tab characters */
    int marker = 0; /* stores the current marker ID */
    char current; /* stores the current character */
    INPUTMODE mode = DEFAULT_MODE; /* stores the current parser state */

    cleararray((char*)&loop, 255);
    loop[0] = '\t';

    ouputboilerplate_before();

    while ((current = fgetc(input)) != EOF) {
        if (mode == SPECIAL_MODE) {
            mode = DEFAULT_MODE;
            if (current >= 'a' && current <= 'f') {
                output("iregisters['%c' - 'a'] = buffer[offset];", loop, current);
            } else if (current >= 'A' && current <= 'F') {
                output("buffer[offset] = iregisters['%c' - 'A'];", loop, current);
            } else if (current >= 's' && current <= 'z') {
                output("lregisters['%c' - 's'] = buffer[offset];", loop, current);
                output("sregisters['%c' - 's'] = (&buffer[offset]) - buffer[offset];", loop, current);
            } else if (current >= 'S' && current <= 'Z') {
                output("if (1) {", loop);
                output("\tunsigned char len = lregisters['%c' - 'S'];", loop, current);
                output("\tregister unsigned char position;", loop);
                output("\tfor (position = 0; position < len; position++) buffer[offset - len + position] = sregisters['%c' - 'S'][position];", loop, current - 'S');
                output("} /* end scope */", loop);
            }

            continue;
        } else if (mode == CHAR_MODE) {
            mode = DEFAULT_MODE;
            output("buffer[offset] = %d;", loop, current);

            continue;
        }

        switch (current) {
        case '>':
            output("offset++;", loop);
            break;
        case '<':
            output("offset--;", loop);
            break;
        case '+':
            output("buffer[offset]++;", loop);
            break;
        case '-':
            output("buffer[offset]--;", loop);
            break;
        case '.':
            output("printf(\"%%c\", buffer[offset]);", loop);
            break;
        case ',':
            output("buffer[offset] = getchar();", loop);
            break;
        case '[': {
            int i;
            output("while (buffer[offset] != 0)", loop);
            output("{", loop);
            for (i = 0; i < 8; i++)
                if (loop[i] != '\t' && (loop[i] = '\t'))
                    break;
            break;
        }
        case ']': {
            int i;
            for (i = 7; i-- > 0;)
                if (loop[i] == '\t' && (loop[i] = 0) == 0)
                    break;
            output("} /* end while */", loop);
            break;
        }
        case '$':
            mode = SPECIAL_MODE;
            break;
        case '%':
            output("printf(\"%%.*s\", lregisters['Z' - 'S'], sregisters['Z' - 'S']);", loop);
            break;
        case '^':
            output("goto marker%d;", loop, marker);
            break;
        case '*':
            marker++;
            output("marker%d:", "", marker);
            break;
        case '\'':
            mode = CHAR_MODE;
            break;
        }
    }

    output("\treturn 0;", "");
    output("}", "");
    output("/* eof */", "");

    return EXIT_SUCCESS;
}
