CC=gcc
CFLAGS= 
OPTS= -O3
LIBS= -lpng -lm -lpthread
OUTFILE=out.png

all: mandelbrot

mandelbrot: mandelbrot.c mandelbrot.h
	$(CC) $(CFLAGS) $(OPTS) mandelbrot.c -o mandelbrot $(LIBS) 

clean:
	rm -f mandelbrot $(OUTFILE)

test: mandelbrot
	./mandelbrot -w 480 -h 480 -o $(OUTFILE)
