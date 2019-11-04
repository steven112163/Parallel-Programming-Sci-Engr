CC=icc
CFLAGS= -std=c99 -Wall -O3
LFLAGS= -lm

all: matrix randMatrix

matrix: matrix.c matrix.h
	$(CC) $(CFLAGS) matrix.h matrix.c -o matrix $(LFLAGS)

randMatrix: randMatrix.c
	$(CC) $(CFLAGS) randMatrix.c -o randMatrix $(LFLAGS)

clean:
	rm -f matrix randMatrix

test: all
	./randMatrix 16 16 | ./matrix --verbose
