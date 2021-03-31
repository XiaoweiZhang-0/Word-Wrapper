CFLAGS=-g -Wall -Werror -fsanitize=undefined -std=c99
wcat: wcat.c
	gcc $(CFLAGS) -o $@ $<
clean:
	rm -f *.o ww
