CC=icc
CFLAGS:= -O3 -w -g $(CFLAGS)
OMPFLAG= -fopenmp -qopenmp
LIBS:= -lm
SRCS= pade.c lubksb.c ludcmp.c mprove.c ratval.c 

all: xpade.run xpade_omp.run

xpade.run: nr.h nrutil.* xpade.c $(SRCS)
	$(CC) $(CFLAGS) xpade.c $(SRCS) nrutil.c $(LIBS) -o xpade.run

xpade_omp.run: nr.h nrutil.* xpade.c $(SRCS)
	$(CC) $(CFLAGS) $(OMPFLAG) xpade.c $(SRCS) nrutil.c $(LIBS) -o xpade_omp.run

clean:
	rm -f xpade*.run

test: test1 test2

test1:
	./xpade.run --verbose 32

test2:
	./xpade_omp.run --verbose 32
