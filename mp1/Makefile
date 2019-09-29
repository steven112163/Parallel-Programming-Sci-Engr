CC=icc
CFLAG= -std=c99 -Wall
OFLAG= -Ofast -xHost -qopt-prefetch -unroll-aggressive -g
DEBUGFLAG= -g

all: matrix matrix_debug randMatrix
debug: matrix_debug

matrix: matrix.c matrix.h
	$(CC) $(CFLAG) $(OFLAG) matrix.h matrix.c -o matrix

matrix_nop: matrix.c matrix.h
	$(CC) $(CFLAG) matrix.h matrix.c -o matrix-nop

matrix_debug: matrix.c matrix.h
	$(CC) $(CFLAG) $(DEBUGFLAG) matrix.h matrix.c -o matrix_debug

clean:
	rm -f matrix matrix-nop matrix_debug
