/**
 * CS - 420 MP 1: Matrix Multiplication
 */

/**
 * Struct for matrix
 */
struct matrix{
    int row;
    int col;
    double **val;
} typedef matrix;

/**
 * Check from the command line if the tile flag is present (-t 100) tile size can differ.
 * @return -1 if no command line argument for tiled is passed or else the size of the tile.
 */
int isTiled(int, char**);

/**
 * Read the matrix from std-in and store it inside the matrix struct.
 * The memory for the val inside the matrix gets allocated.
 * The input is in the following format. The comments inside the parentheses are only for explanation
 * and not part of the input :
 * 2 3  (rox x col of the 1st matrix)
 * 1 2 3
 * 5 6 7
 * 3 2 (row x col of the 2nd matrix)
 * 7 8
 * 9 10
 * 11 12
 */
void readMatrix(matrix*);

/**
 * Multiply the matrix using the naive matrix multiplication technique.
 * Should call printResults with the respective metrics after the multiplication is complete.
 */
void matrixMultiply(matrix, matrix, matrix*);

/**
 * Multiply the matrix using the tiled / block matrix multiplication technique.
 * Should call printResults with the respective metrics after the multiplication is complete.
 */
void matrixTiledMultiply(matrix, matrix, matrix*, int);

/**
 * Print the analytics i.e. Total number of flops(Floating point operations) & MegaFlops per second.
 */
void printAnalytics(long, double);

/**
 * Check from the command line if only analytics needs to be printed
 * @return -1 if -oa flag is not present or else 1
 */
int onlyAnalytics(int, char**);

/**
 * Print the matrix to std out.
 * The output is in the following format:
 * 2 2 (rox x col of the multiplication result matrix)
 * 1 2
 * 3 4
 */
void printMatrix(matrix);

/**
 * Print that there is an error and exit the program.
 */
void error();
