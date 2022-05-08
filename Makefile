CC=gcc

BIN=bin/compress bin/decompress

SRCS0=compress.c
SRCS1=decompress.c

default: all

all: $(BIN)

obj/%.o: %.c
	$(CC) -Wall -Iinclude -c $< -o $@

bin/compress: $(SRCS0:%.c=obj/%.o)
	$(CC) -o $@ $+

bin/decompress: $(SRCS1:%.c=obj/%.o)
	$(CC) -o $@ $+

clean:
	rm -f $(BIN) obj/*.o *~
