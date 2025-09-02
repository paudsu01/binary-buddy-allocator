CC=gcc
target: buddy.o
	gcc -shared -o libmem1.so buddy.o

buddy.o: buddy.c buddy.h
	$(CC) -c -fpic buddy.c -Wall -Werror

clean:
	rm *.o
	rm *.so
