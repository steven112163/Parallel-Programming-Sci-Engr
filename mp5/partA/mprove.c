
#define NRANSI
#include "nrutil.h"

void mprove(float **a, float **alud, int n, int indx[], float b[], float x[])
{
	void lubksb(float **a, int n, int *indx, float b[]);
	int j, i;
	double sdp;
	float *r;

	r = vector(1,n);
  #pragma omp parallel for simd shared(n, b, a, x, r) private(i, sdp, j)
  for (i = 1; i <= n; i++) {
		sdp = -b[i];
		for (j = 1; j <= n; j++) sdp += a[i][j]*x[j];
		r[i] = sdp;
	}
  #pragma omp barrier
	lubksb(alud, n, indx, r);
  for (i = 1; i <= n; i++) x[i] -= r[i];
	free_vector(r, 1, n);
}
#undef NRANSI
