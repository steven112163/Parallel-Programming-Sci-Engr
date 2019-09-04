/**
 * CS - 420 MP 1: Matrix Multiplication
 */

#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"

int main(int argc, char** argv) {
    matrix m1;
    matrix m2;
    matrix result;

    int tiled = isTiled(argc, argv);

    readMatrix(&m1);
    readMatrix(&m2);

    //TODO: Check for error conditions

    if(tiled == -1) matrixMultiply(m1, m2, &result);
    else    matrixTiledMultiply(m1, m2, &result, tiled);

    if(onlyAnalytics(argc, argv) == -1) printMatrix(result);
    return 0;
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


void readMatrix(matrix* mat){
    scanf("%d", &mat -> row);
    scanf("%d", &mat -> col);

    mat->val = (double**) malloc(sizeof(double*) * mat->row);

    for(int n=0; n < mat->row; n++){
        mat->val[n] = (double*) malloc(sizeof(double*) * mat->col);
        for(int m=0; m < mat->col; m++){
            scanf("%lf", &mat->val[n][m]);
        }
    }
}


void matrixMultiply(matrix m1, matrix m2, matrix* result){
   //TODO: Naive Matrix Multiplication
}

void matrixTiledMultiply(matrix m1, matrix m2, matrix *result, int tileSize) {
    //TODO: Tiled Matrix Multiplication
}

void error(){
    printf("Error. Exiting the program.\n");
    exit(1);
}


void printAnalytics(long ops, double mFlops){
    printf("Operations: %ld and MegaFlops/s:%lf\n",ops, mFlops);
}

int onlyAnalytics(int argc, char **argv){
    int oaFlag = -1;
    for(int n=1; n < argc; n++){
        if(strcmp(argv[n], "-oa") == 0){
            oaFlag = 1;
            break;
        }
    }
    return oaFlag;
}

void printMatrix(matrix mat){
    printf("%d %d\n", mat.row, mat.col);
    for(int n=0; n < mat.row; n++){
        for(int m=0; m < mat.col; m++){
            printf("%lf ", mat.val[n][m]);
        }
        printf("\n");
    }
}