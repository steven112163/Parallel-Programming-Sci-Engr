#ifndef STENCIL2D_H
#define STENCIL2D_H

// Rank of the current PE
extern int kRank;
// Total number of PE's
extern int kNumPes;
// Number of iterations to run for
extern int kNumIts;
// Dimensions of the PE grid
extern int kGridRows, kGridCols;
// A PE's position in the grid
extern int kGridX, kGridY;
// Dimensions of the stencil computation
extern int kWidth, kHeight;
// Initializes a m by n stencil tile with values, calculated according to the
// stencil's position in the grid
void init_stencil(int xOff, int yOff, int m, int n, float a[m + 2][n + 2]);
// Performs a 2d, five-point w x h stencil on in, storing the result in out.
void stencil_2d(int w, int h, float in[w + 2][h + 2], float out[w + 2][h + 2]);
// Copies a 2d stencil into a 1d buffer, discarding the padding
void copy_stencil_to_buffer(int w, int h, float in[w + 2][h + 2],
                            float out[w][h]);
// Copies a 1d buffer into a 2d stencil, at offset (x,y)
void copy_buffer_to_stencil(int w, int h, float dst[w][h], int ww, int hh,
                            float src[ww][hh], int x, int y);
// Prints a matrix to stdout
void print_matrix(int m, int n, float a[m][n]);
// Swaps the values of a and b
void swap(float **a, float **b);
// Finds the largest prime factor of an integer
int max_prime_factor(int n);
// Sets up the enviroment, initializing MPI and the global variables
void setup_env(int argc, char *argv[]);
// Row and column buffer helper functions. Copies a value into/out of the PE's
// tile from a buffer, with an x/y offset
void copy_rowbuf_out(int w, int h, float src[w + 2][h + 2], int y,
                     float rowBuf[w]);
void copy_colbuf_out(int w, int h, float src[w + 2][h + 2], int x,
                     float colBuf[h]);
void copy_rowbuf_in(int w, int h, float dst[w + 2][h + 2], int y,
                    float rowBuf[w]);
void copy_colbuf_in(int w, int h, float dst[w + 2][h + 2], int x,
                    float colBuf[h]);
// Sends the ghost values with a tag, using row/colBuf for temporary storage
void send_ghosts(int tag, int w, int h, float src[w + 2][h + 2],
                 float rowBuf[w], float colBuf[h]);
// Receives the ghost values with a tag, using row/colBuf for temporary storage
void recv_ghosts(int tag, int w, int h, float dst[w + 2][h + 2],
                 float rowBuf[w], float colBuf[h]);

#endif
