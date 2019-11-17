CC=mpicc

ifeq ($(VERBOSE_LEVEL),)
VERBOSE_LEVEL := 0
endif

all: stencil2d

stencil2d: stencil2d.c stencil2d.h
	$(CC) stencil2d.c -o stencil2d -DVERBOSE_LEVEL=$(VERBOSE_LEVEL)

test: one multi

one:
	mpirun -np 1 ./stencil2d 16 16 1

multi:
	mpirun -np 4 ./stencil2d 16 16 1

clean:
	rm -f stencil2d
