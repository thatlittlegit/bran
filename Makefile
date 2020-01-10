WARNINGS=-Wall -Werror -Wpedantic
CFLAGS=-ansi $(WARNINGS) $(DEBUG)

all: bran

clean:
	rm -f bran

format:
	clang-format -style=WebKit -i bran.c

install:
	install -v -D bran -t /usr/local/bin -m 755
	install -v -D bran.1 -t /usr/local/share/man/man1 -m 644
