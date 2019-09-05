all:            bran-asm bran-c bran-compile
bran-asm:       bran-asm.o
bran-c:         bran-c.o
libbran.o:      bran-c.c
	$(CC) $(CFLAGS) -c -DLIBRARY -o $@ $^
clean:
	-rm -f bran-asm bran-c *.o
