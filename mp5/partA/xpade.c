
/* Driver for routine pade */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#define NRANSI
#include "nr.h"
#include "nrutil.h"

double fn(double x)
{
    return (x == 0.0 ? 1.0 : log(1.0 + x) / x);
}

int main(int argc, char* argv[])
{
    int j, k, n = 2048, verbose = 0;
    float resid;
    double b, d, fac, x, *c;

    for (j = 0; j < argc; j++) {
        if (strcmp(argv[j], "--verbose") == 0) {
            verbose = 1;
        }
        else {
            n = atoi(argv[j]);
        }
    }

    c = dvector(0, 2 * n + 1);
    fac = 1;

    for (j = 1; j <= 2 * n + 1; j++) {
        c[j - 1] = fac / ((double)j);
        fac = -fac;
    }

    pade(c, n, &resid);

    if (verbose) {
        printf("Norm of residual vector= %16.8e\n", resid);
        printf("point, func. value, pade series:\n");
        for (j = 1; j <= 21; j++) {
            x = (j - 1) * 0.25;
            d = ratval(x, c, n, n);
            printf("%16.8f\t%16.8f\t%16.8f\n", x, fn(x), d);
        }
    }

    free_dvector(c, 0, 2 * n + 1);
    return 0;
}

#undef NRANSI
