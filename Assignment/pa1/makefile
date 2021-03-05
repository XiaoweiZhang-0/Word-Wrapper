CFLAGS=-g -Wall -Werror -fsanitize=address -std=c99
ww: ww.c
	gcc $(CFLAGS) -o $@ $<
clean:
	rm -f *.o ww
