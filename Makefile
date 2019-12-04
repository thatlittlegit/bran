WARNINGS=-Wall -Werror -Wpedantic
CFLAGS=-ansi $(WARNINGS)

all: bran

clean:
	rm -f bran

format:
	clang-format -style=WebKit -i bran.c
