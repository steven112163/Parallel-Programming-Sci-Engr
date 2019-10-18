CC=gcc
CFLAGS=-std=c11 
OPTS= -O3
LIBS= -lpthread

all: histogram

histogram: histogram.c histogram.h
	$(CC) $(CFLAGS) $(OPTS) histogram.c -o histogram $(LIBS) 

clean:
	rm -f histogram

test: histogram
	./histogram -v
