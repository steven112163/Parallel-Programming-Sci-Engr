#include "matrix.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

// NOTE: Reference implementation of Dense MVM
float* mvm(int m, int n, float a[m][n], float b[n])
{
    float* c = (float*)malloc(sizeof(float) * m);
    int i, j;
    for (i = 0; i < m; i++) {
        c[i] = 0.0f;
        for (j = 0; j < n; j++) {
            c[i] += a[i][j] * b[j];
        }
    }
    return c;
}

float* smvm_crs(crs_t* a, float* b)
{
    float* c = (float*)malloc(sizeof(float) * (a->m));
   /* TODO:
    * Implement SVM_CRS from Page 87 in the Textbook.
    * NOTE:
    * a->ia corresponds to row_ptr
    * a->ja corresponds to col_idx
    * a->a  corresponds to val
    */
    return c;
}

float* smvm_jds(jds_t* a, float* b)
{
    float* c = (float*)malloc(sizeof(float) * (a->m));
   /* TODO:
    * Implement SVM_JDS from Page 88 in the Textbook.
    * NOTE:
    * The code in the textbook will compute a permutation of the product vector
    * since the rows of the matrix have been permuted. You can permute the result
    * to the correct ordering after performing sMVM or use c[a->perm[...]]
    * instead of c[...] while performing sMVM.
    */
    return c;
}

double get_wall_time()
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return tp.tv_sec + (tp.tv_usec / 1e6);
}

int main(int argc, char** argv)
{
    int m, n, vn;
    float* a = read_matrix(&m, &n);
    float* v = read_vector(&vn);
    assert(vn == n);
    crs_t* crs = matrix_to_crs(m, n, (float(*)[n])a);
    jds_t* jds = matrix_to_jds(m, n, (float(*)[n])a);
    double t1 = get_wall_time();
    float* c1 = mvm(m, n, (float(*)[n])a, v);
    double t2 = get_wall_time();
    float* c2 = smvm_crs(crs, v);
    double t3 = get_wall_time();
    float* c3 = smvm_jds(jds, v);
    double t4 = get_wall_time();
    if ((argc == 2) && (strcmp(argv[1], "--verbose") == 0)) {
        printf("c1 = ");
        print_vector(m, c1);
        printf("c2 = ");
        print_vector(m, c2);
        printf("c3 = ");
        print_vector(m, c3);
    }
    assert(same(n, c1, c2, 1e-6));
    assert(same(n, c2, c3, 1e-6));
    printf("Dense MVM (reference) took %.3lf ms.\n", 1e3 * (t2 - t1));
    printf("CRS sMVM took %.3lf ms.\n", 1e3 * (t3 - t2));
    printf("JDS sMVM took %.3lf ms.\n", 1e3 * (t4 - t3));
    free(a);
    free(v);
    free(c1);
    free(c2);
    free(c3);
    free_crs(crs);
    free_jds(jds);
    return 0;
}

int count_nnz(int m, int n, float a[m][n])
{
    int i, j, nnz = 0;
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            if (a[i][j] != 0.0f) {
                nnz++;
            }
        }
    }
    return nnz;
}

crs_t* matrix_to_crs(int m, int n, float a[m][n])
{
    crs_t* crs = (crs_t*)malloc(sizeof(crs_t));
    crs->m = m;
    crs->n = n;
    crs->nnz = count_nnz(m, n, a);
    crs->a = (float*)malloc(sizeof(float) * crs->nnz);
    crs->ia = (int*)malloc(sizeof(int) * (m + 1));
    crs->ja = (int*)malloc(sizeof(int) * crs->nnz);
    int i, j, nnz = 0;
    float* ait = crs->a;
    int *jait = crs->ja, *iait = crs->ia;
    *(iait++) = 0;
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            if (a[i][j] != 0.0f) {
                *(ait++) = a[i][j];
                *(jait++) = j;
                nnz++;
            }
        }
        *(iait++) = nnz;
    }
    return crs;
}

float* read_matrix(int* m, int* n)
{
    scanf("%d %d", m, n);
    float* a = malloc(sizeof(float) * (*m) * (*n));
    int i, j;
    for (i = 0; i < *m; i++) {
        for (j = 0; j < *n; j++) {
            scanf("%f", &a[i * (*m) + j]);
        }
    }
    return a;
}

float* read_vector(int* n)
{
    scanf("%d", n);
    float* a = malloc(sizeof(float) * (*n));
    int i;
    for (i = 0; i < *n; i++) {
        scanf("%f", &a[i]);
    }
    return a;
}

void print_vector(int n, float a[n])
{
    int i;
    for (i = 0; i < n; i++) {
        printf("%f\t", a[i]);
    }
    printf("\n");
}

void print_matrix(int m, int n, float a[m][n])
{
    int i;
    for (i = 0; i < m; i++) {
        print_vector(n, (float*)&a[i]);
    }
}

void free_crs(crs_t* crs)
{
    free(crs->a);
    free(crs->ia);
    free(crs->ja);
    free(crs);
}

void free_jds(jds_t* jds)
{
    free(jds->a);
    free(jds->col_idx);
    free(jds->jd_ptr);
    free(jds->perm);
    free(jds);
}

int same(int n, float* a, float* b, float tol)
{
    int i = 0, flag = 1;
    for (i = 0; flag && (i < n); i++) {
        flag = flag && (fabs(a[i] - b[i]) <= tol);
    }
    return flag;
}

void permute(int m, int n, float a[m][n], jds_t* jds)
{
    int* nnz = (int*)malloc(sizeof(int) * m);
    int i, j;
    jds->nnz = 0;
    for (i = 0; i < m; i++) {
        nnz[i] = 0;
        for (j = 0; j < n; j++) {
            if (a[i][j] != 0.0f) {
                nnz[i]++;
            }
        }
        jds->nnz += nnz[i];
    }
    for (i = 0; i < m; i++) {
        int max = 0;
        for (j = 1; j < m; j++) {
            max = (nnz[j] > nnz[max]) ? j : max;
        }
        if (i == 0) {
            jds->nj = nnz[max];
        }
        jds->perm[i] = max;
        nnz[max] = -1;
    }
    free(nnz);
}

jds_t* matrix_to_jds(int m, int n, float a[m][n])
{
    jds_t* jds = (jds_t*)malloc(sizeof(jds_t));
    jds->perm = (int*)malloc(sizeof(int) * m);
    permute(m, n, a, jds);
    jds->jd_ptr = (int*)malloc(sizeof(int) * (jds->nj + 1));
    jds->a = (float*)malloc(sizeof(float) * jds->nnz);
    jds->col_idx = (int*)malloc(sizeof(int) * jds->nnz);
    jds->m = m;
    jds->n = n;
    float* at = jds->a;
    int* ct = jds->col_idx;
    int* jt = jds->jd_ptr;
    *jt = 0;
    jt++;
    int i, j, k;
    for (i = 0; i < jds->nj; i++) {
        *jt = 0;
        for (j = 0; j < m; j++) {
            int skip = i;
            for (k = 0; k < n; k++) {
                if (a[jds->perm[j]][k] == 0.0f) {
                    continue;
                }
                else if (skip > 0) {
                    skip--;
                    continue;
                }
                else {
                    *(at++) = a[jds->perm[j]][k];
                    *(ct++) = k;
                    (*jt)++;
                    break;
                }
            }
        }
        *jt += *(jt - 1);
        jt++;
    }
    return jds;
}
