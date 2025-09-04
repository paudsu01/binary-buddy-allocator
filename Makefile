CC=gcc
CFLAGS=-g -Wall -Werror -fpic

TARGET=libmem1.so
OBJS=buddy.o utils.o

$(TARGET): $(OBJS)
	$(CC) -shared -o $@ $(OBJS)

buddy.o: buddy.c buddy.h utils.h types.h
	$(CC) $(CFLAGS) -c buddy.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f $(OBJS) $(TARGET)

