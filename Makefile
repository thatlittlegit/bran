WARNINGS=-Wall -Werror -Wpedantic
CFLAGS=-ansi $(WARNINGS) $(DEBUG)

all: bran

clean:
	rm -f bran

format:
	clang-format -style=WebKit -i bran.c
