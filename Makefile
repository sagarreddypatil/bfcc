CC = gcc

all: bin/out

bin/out: bin/gen.s
	$(CC) bootstrap.c bin/gen.s -o bin/out

bin/gen.s: bin/bfcc test.bf
	bin/bfcc test.bf > bin/gen.s

bin/bfcc: bin
	$(CC) -o bin/bfcc bfcc.c

clean:
	rm -f bin/bfcc

bin:
	mkdir bin