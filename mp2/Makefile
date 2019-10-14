CC=icc
CFLAG= -std=c99 -Wall
OFLAG= -Ofast -xHost -qopt-prefetch -unroll-aggressive -g

all: matrix

matrix: matrix.c matrix.h
	$(CC) $(CFLAG) $(OFLAG) matrix.h matrix.c -o matrix

clean:
	rm -f matrix