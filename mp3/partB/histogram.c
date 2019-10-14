#include "histogram.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <getopt.h>
#include <pthread.h>

char* data;
char* sorted;
int counts[26];

int main(int argc, char* argv[])
{
    // initialize defaults
    int len = 128;
    int seed = 123456;
    int numThreads = 0;
    int verbose = 0;
    // parse arguments
    int c;
    while ((c = getopt(argc, argv, "n:s:l:v")) != -1) {
        switch (c) {
        case 'n':
            numThreads = atoi(optarg);
            break;
        case 's':
            seed = atoi(optarg);
            break;
        case 'l':
            len = atoi(optarg);
            break;
        case 'v':
            verbose = 1;
            break;
        default:
            abort();
        }
    }
    srand(seed);
    data = generate_random_string(len);
    sorted = (char*)malloc(len * sizeof(char));
    if (numThreads) {
        // TODO : launch numThreads threads and divvy up the compute counts workload
    }
    else {
        compute_counts(0, len);
    }
    populate_sorted();
    if (verbose) {
        printf("unsorted list: %s\n", data);
        printf("sorted list: %s\n", sorted);
    }
    free(data);
    free(sorted);
    return 0;
}

char* generate_random_string(int len)
{
    int i;
    char* str = (char*)malloc(len * sizeof(char));
    for (i = 0; i < len; i++) {
        str[i] = (rand() % 26) + 'A';
    }
    return str;
}

void compute_counts(int start, int end)
{
    int i;
    for (i = start; i < end; i++) {
        // TODO replace with an atomic add
        counts[data[i] - 'A'] += 1;
    }
}

void populate_sorted()
{
    int i = 0, c;
    for (c = 0; c < 26; c++) {
        for (; counts[c]-- > 0; i++) {
            sorted[i] = c + 'A';
        }
    }
}
