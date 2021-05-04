CFLAGS=-g -Wall -Werror -fsanitize=address,undefined -std=c99 
LFLAGS=-lm -lpthread

main: main.c
	gcc $(CFLAGS) -o $@ $< $(LFLAGS)

clean:
	rm -f *.o main