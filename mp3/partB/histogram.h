#ifndef HISTOGRAM_H
#define HISTOGRAM_H

extern char *data;
extern char *sorted;
extern int counts[26];

char* generate_random_string(int len);
void compute_counts(int start, int end);
void populate_sorted();

#endif
