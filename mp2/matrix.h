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
matrix* readMatrix();

/**
 * Multiply the matrix using the tiled / block matrix multiplication technique using vector instructions.
 * Should call printResults with the respective metrics after the multiplication is complete.
 */
matrix* tiledMatMul(matrix*, matrix*, int);
void luDecomposition(matrix*, matrix*, matrix*);

/**
 * Print the analytics i.e. Total number of flops(Floating point operations) & MegaFlops per second.
 */
void printAnalytics(long, double);

/**
 * Check from the command line if only analytics needs to be printed
 * @return -1 if -oa flag is not present or else 1
 */
int onlyAnalytics(int, char**);

int isLu(int, char**);

/**
 * Print the matrix to std out.
 * The output is in the following format:
 * 2 2 (rox x col of the multiplication result matrix)
 * 1 2
 * 3 4
 */
void printMatrix(matrix*);

/**
 * Print that there is an error and exit the program.
 */
void error();

/**
 * Free memory allocated for a matrix pointer.
 */
void freeMatrix(matrix*);

/**
 * Create a new matrix of the specified size.
 */
matrix* newMatrix(int, int);

/**
 * Get the time.
 */
void get_walltime_(double*);
void get_walltime(double*);