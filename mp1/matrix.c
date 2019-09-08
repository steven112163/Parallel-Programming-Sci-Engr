/**
 * CS - 420 MP 1: Matrix Multiplication
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "matrix.h"

int main(int argc, char** argv) {
    matrix m1;
    matrix m2;
    matrix result;

    int tiled = isTiled(argc, argv);

    readMatrix(&m1);
    readMatrix(&m2);

    //TODO: Check for error conditions
    if(m1.col != m2.row) error();

    result.row = m1.row;
    result.col = m2.col;
    result.val = (double**) malloc(sizeof(double*) * result.row);
    for(int i = 0; i < result.row; i++) {
        result.val[i] = (double*) malloc(sizeof(double*) * result.col);
        for(int j = 0; j < result.col; j++)
            result.val[i][j] = 0.0;
    }
    
    double startTime, endTime;
    long ops;
    get_walltime(&startTime);
    
    if(tiled == -1) matrixMultiply(m1, m2, &result);
    else {
        if(m1.row%tiled != 0 || m1.col%tiled != 0 || m1.row != m1.col) error();
        if(m2.row%tiled != 0 || m2.col%tiled != 0 || m2.row != m2.col) error();
        matrixTiledMultiply(m1, m2, &result, tiled);
    }
    
    get_walltime(&endTime);
    if(tiled == -1) ops = result.row * result.col * m2.row * 2;
    else ops = (m1.row / tiled) * (m1.row / tiled) * (m1.row / tiled) * tiled * tiled * tiled * 2;
    printAnalytics(ops, ops/(endTime - startTime)/1000000.0);

    if(onlyAnalytics(argc, argv) == -1) printMatrix(result);
    return 0;
}


int isTiled(int argc, char** argv) {
    int tileSize = -1;
    for(int n = 1; n < argc; n++){
        if(strcmp(argv[n], "-t") == 0){
            tileSize = atoi(argv[n+1]);
            break;
        }
    }
    return tileSize;
}


void readMatrix(matrix* mat) {
    scanf("%d", &mat -> row);
    scanf("%d", &mat -> col);

    mat->val = (double**) malloc(sizeof(double*) * mat->row);

    for(int n = 0; n < mat->row; n++) {
        mat->val[n] = (double*) malloc(sizeof(double*) * mat->col);
        for(int m = 0; m < mat->col; m++) {
            scanf("%lf", &mat->val[n][m]);
        }
    }
}


void matrixMultiply(matrix m1, matrix m2, matrix* result) {
   //TODO: Naive Matrix Multiplication
   for(int i = 0; i < result->row; i++)
       for(int j = 0; j < result->col; j++)
           for(int k = 0; k < m2.row; k++)
               result->val[i][j] += m1.val[i][k] * m2.val[k][j];

   return;
}

void matrixTiledMultiply(matrix m1, matrix m2, matrix* result, int tileSize) {
    //TODO: Tiled Matrix Multiplication
    int numOfTiles = m1.row / tileSize;
    
    for(int rowOfBlocks = 0; rowOfBlocks < numOfTiles; rowOfBlocks++)
        for(int colOfBlocks = 0; colOfBlocks < numOfTiles; colOfBlocks++)
            for(int innerBlocks = 0; innerBlocks < numOfTiles; innerBlocks++) {
                int rowOffset = rowOfBlocks*tileSize,
                    colOffset = colOfBlocks*tileSize,
                    innOffset = innerBlocks*tileSize;
                for(int i = rowOffset; i < rowOffset + tileSize; i++)
                    for(int j = colOffset; j < colOffset + tileSize; j++)
                        for(int k = innOffset; k < innOffset + tileSize; k++)
                            result->val[i][j] += m1.val[i][k] * m2.val[k][j];
            }
    
    return;
}

void error() {
    printf("Error. Exiting the program.\n");
    exit(1);
}


void printAnalytics(long ops, double mFlops) {
    printf("Operations: %ld and MegaFlops/s:%lf\n", ops, mFlops);
}

int onlyAnalytics(int argc, char **argv) {
    int oaFlag = -1;
    for(int n = 1; n < argc; n++) {
        if(strcmp(argv[n], "-oa") == 0) {
            oaFlag = 1;
            break;
        }
    }
    return oaFlag;
}

void printMatrix(matrix mat) {
    printf("%d %d\n", mat.row, mat.col);
    for(int n = 0; n < mat.row; n++) {
        for(int m = 0; m < mat.col; m++) {
            printf("%lf ", mat.val[n][m]);
        }
        printf("\n");
    }
}

void get_walltime_(double* wcTime) {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    *wcTime = (double)(tp.tv_sec + tp.tv_usec/1000000.0);
}

void get_walltime(double* wcTime) {
    get_walltime_(wcTime);
}