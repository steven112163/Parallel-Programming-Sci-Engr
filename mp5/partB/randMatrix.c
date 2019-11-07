#include <stdio.h>
#include <stdlib.h>

void generateSparseMatrix(int, int);

const int zeroProb = 50;

int main(int argc, char **argv){
    if(argc != 3) {
        printf("Usage is: %s <m> <n>\n", argv[0]);
        exit(1);
    }

    int m = atoi(argv[1]);
    int n = atoi(argv[2]);

    printf("%d %d\n", m, n);
    generateSparseMatrix(m, n);
    printf("%d\n", n);
    generateSparseMatrix(1, n);

    return 0;
}

void generateSparseMatrix(int row, int col) {
    for(int n=0; n < row; n++) {
        for(int m=0; m < col; m++) {
            int zero = (rand() % 100) >= (100 - zeroProb);
            int sign = (rand() % 2) ? -1 : 1;
            int num  = zero ? 0 : (sign * (rand() % 100)); 
            printf("%d\t", num);
        }
        printf("\n");
    }
}
