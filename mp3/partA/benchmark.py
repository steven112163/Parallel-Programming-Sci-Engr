#!/usr/bin/python3
from sys import argv
from multiprocessing import cpu_count
from timeit import timeit

def even_factors(x):
    for i in range(2, x + 1, 2):
        if x % i == 0 and i % 2 == 0:
            yield i

reps  = 1
size  = 2048 if len(argv) == 1 else int(argv[1])
cores = sorted([1, cpu_count() + 4, cpu_count() + 8] + list(even_factors(cpu_count())))

def run_mandelbrot(tc = None):
    args = "./mandelbrot -w %d -h %d " % (size, size)
    if (tc):
        args += "-n %d" % tc
    return timeit(stmt="subprocess.call(\"%s\", shell=True)" % args, setup="import subprocess", number=reps) / reps

print("Generating a {0}x{0} mandelbrot with thread counts: {1}".format(size, cores))
serial = run_mandelbrot()
print("The serial version ran for",serial,"s.")
for core in cores:
    time = run_mandelbrot(core)
    print("The parallel version, with",core,"thread(s), ran for",time,"s, a speedup of %.3fx." % (serial/time))