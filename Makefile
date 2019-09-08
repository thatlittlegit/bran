all:            bran-c

bran-c:         bran-c.o
libbran.o:      bran-c.c
	$(CC) $(CFLAGS) -c -DLIBRARY -o $@ $^
clean:
	-rm -f bran-c *.o
