CC=icc
CFLAGS:= -O3 -w $(CFLAGS)
OMPFLAG= -fopenmp
LIBS:= -lm
SRCS= pade.c lubksb.c ludcmp.c mprove.c ratval.c 

all: xpade.run xpade_omp.run

xpade.run: nr.h nrutil.* xpade.c $(SRCS)
	$(CC) $(CFLAGS) xpade.c $(SRCS) nrutil.c $(LIBS) -o xpade.run

xpade_omp.run: nr.h nrutil.* xpade.c $(SRCS)
	$(CC) $(CFLAGS) $(OMPFLAG) xpade.c $(SRCS) nrutil.c $(LIBS) -o xpade_omp.run

clean:
	rm -f xpade*.run

test: all
	./xpade.run --verbose 32
