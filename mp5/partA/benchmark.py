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
cores = sorted([1, cpu_count() + 4] + list(even_factors(cpu_count())))

def run_xpade(tc = None):
    if not tc:
        args = "./xpade.run"
    else:
        args = "./xpade_omp.run"
    args += " %d" % size
    return timeit(stmt="subprocess.call(\"%s\", shell=True, env=dict(os.environ, OMP_NUM_THREADS=\"%d\"))" % (args, tc or 1), setup="import os, subprocess", number=reps) / reps

print("Running xpade for size {0} with thread counts: {1}".format(size, cores))
serial = run_xpade()
print("The serial version ran for",serial,"s.")
for core in cores:
    time = run_xpade(core)
    print("The parallel version, with",core,"thread(s), ran for",time,"s, a speedup of %.3fx." % (serial/time))
