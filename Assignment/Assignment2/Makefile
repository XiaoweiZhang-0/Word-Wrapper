cflag = -g -std=c99 -Wvla -Wall -fsanitize=address
test: main.c strbuf.o
	gcc $(cflag) -o test main.c strbuf.o
str.o: strbuf.c strbuf.h
	gcc -c $(cflag) strbuf.c
main.c: strbuf.h


clean:
	rm -f *.o test dd