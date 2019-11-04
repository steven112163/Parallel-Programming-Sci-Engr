#ifndef MATRIX_H
#define MATRIX_H

typedef struct crs {
    float *a;
    int *ia, *ja;
    int nnz, m, n;
} crs_t;

typedef struct jds {
    float *a;
    int *jd_ptr, *col_idx, *perm;
    int nnz, nj, m, n;
} jds_t;

int count_nnz(int m, int n, float a[m][n]);
int same(int n, float *a, float *b, float tol);
void permute(int m, int n, float a[m][n], jds_t *jds);

jds_t* matrix_to_jds(int m, int n, float a[m][n]);
crs_t* matrix_to_crs(int m, int n, float a[m][n]);

void free_crs(crs_t*);
void free_jds(jds_t*);

float* read_matrix(int *m, int *n);
float* read_vector(int *n);

void print_vector(int n, float a[n]);
void print_matrix(int m, int n, float a[m][n]);

float* mvm(int m, int n, float a[m][n], float b[n]);
float* smvm_crs(crs_t* a, float* b);
float* smvm_jds(jds_t* a, float* b);

#endif