/**
 * CS - 420 MP 2: Matrix Vectorized Multiplication
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int main(int argc, char** argv) {
    int tileSize = isTiled(argc, argv);
    if (tileSize == -1 && !isLu(argc, argv)) error();

    if (isLu(argc, argv)) {
        matrix *a = readMatrix();
        if (a->row != a->col) {
            error();
        }
        matrix *l = newMatrix(a->row, a->col);
        matrix *u = newMatrix(a->row, a->col);
        luDecomposition(a, l, u);
        if (!onlyAnalytics(argc, argv)) {
            printMatrix(l); printMatrix(u);
        }
        freeMatrix(a); freeMatrix(l); freeMatrix(u);
    } else {
        matrix *a = readMatrix();
        matrix *b = readMatrix();
        matrix *c = tiledMatMul(a, b, tileSize);
        if (!onlyAnalytics(argc, argv)) {
            printMatrix(c);
        }
        freeMatrix(a); freeMatrix(b); freeMatrix(c);
    }

    return 0;
}

int isLu(int argc, char **argv){
    int luFlag = 0;
    for(int n=1; n < argc; n++){
        if(strcmp(argv[n], "-lu") == 0){
            luFlag = 1;
            break;
        }
    }
    return luFlag;
}

int isTiled(int argc, char** argv){
    int tileSize = -1;
    for(int n=1; n < argc; n++){
        if(strcmp(argv[n], "-t") == 0){
            tileSize = atoi(argv[n+1]);
            break;
        }
    }
    return tileSize;
}

matrix* newMatrix(int numRows, int numCols) {
    matrix* mat = (matrix*) malloc(sizeof(matrix));
    mat->row = numRows;
    mat->col = numCols;
    mat->val = (double**) malloc(sizeof(double*) * mat->row);
    for(int i=0; i < mat->row; i++) {
        mat->val[i] = (double*) malloc(sizeof(double) * mat->col);
        for (int j = 0; j < mat->col; j++) {
            mat->val[i][j] = 0;
        }
    }
    return mat;
}

void freeMatrix(matrix* mat) {
    for (int i = 0; i < mat->row; i++) {
        free(mat->val[i]);
    }
    free(mat->val);
    free(mat);
}

matrix* readMatrix(){
    int numRows, numCols;
    scanf("%d", &numRows);
    scanf("%d", &numCols);
    matrix* mat = newMatrix(numRows, numCols);
    for(int n=0; n < mat->row; n++){
        for(int m=0; m < mat->col; m++){
            scanf("%lf", &mat->val[n][m]);
        }
    }
    return mat;
}

matrix* tiledMatMul(matrix *m1, matrix *m2, int tileSize) {
    matrix *result = newMatrix(m1->row, m2->col);

    /* TODO : implement tiled matrix-matrix multiplication
     *        with intrinsics
     */

    return result;
}

void luDecomposition(matrix* a, matrix* l, matrix* u) {
    // TODO : implement lu decomposition with intrinsics
    int i, j, k;
    for (i = 0; i < a->col; i++) {
        for (j = 0; j < a->col; j++) {
            if (j < i) {
                l->val[j][i] = 0;
            } else {
                l->val[j][i] = a->val[j][i];
                for (k = 0; k < i; k++) {
                    l->val[j][i] = l->val[j][i] - l->val[j][k] * u->val[k][i];
                }
            }
        }
        for (j = 0; j < a->col; j++) {
            if (j < i) {
                u->val[i][j] = 0;
            } else if (j == i) {
                u->val[i][j] = 1;
            } else {
                u->val[i][j] = a->val[i][j] / l->val[i][i];
                for (k = 0; k < i; k++) {
                    u->val[i][j] = u->val[i][j] - ((l->val[i][k] * u->val[k][j]) / l->val[i][i]);
                }
            }
        }
    }
}

void error(){
    printf("Error. Exiting the program.\n");
    exit(1);
}

void printAnalytics(long ops, double mFlops){
    printf("Operations: %ld and MegaFlops/s:%lf\n",ops, mFlops);
}

int onlyAnalytics(int argc, char **argv){
    int oaFlag = 0;
    for(int n=1; n < argc; n++){
        if(strcmp(argv[n], "-oa") == 0) {
            oaFlag = 1;
            break;
        }
    }
    return oaFlag;
}

void printMatrix(matrix *mat){
    printf("%d %d\n", mat->row, mat->col);
    for(int n=0; n < mat->row; n++){
        for(int m=0; m < mat->col; m++){
            printf("%lf ", mat->val[n][m]);
        }
        printf("\n");
    }
}