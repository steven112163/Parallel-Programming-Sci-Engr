#include "stencil2d.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <mpi.h>
#include <sys/time.h>

#ifndef VERBOSE_LEVEL
#define VERBOSE_LEVEL 0
#endif

#define SCALING_FACTOR 0.125f

void send_ghosts(int tag, int w, int h, float src[w + 2][h + 2],
                 float rowBuf[w], float colBuf[h]) {
  /* TODO:
   * Send values from src to the appropriate neighbors, using
   * rowBuf and colBuf as the send buffers and tag as the tag
   * for the messages. You may use the provided copy buffer
   * helper functions for this task. Use the grid information
   * to determine which ranks to send values to. 
   */
  if (0 <= kGridX - 1) {
    // has top
    copy_colbuf_out(w, h, src, 1, colBuf);
    MPI_Send(colBuf, h, MPI_FLOAT, kRank - kGridCols, tag, MPI_COMM_WORLD);
  }
  if (kGridX + 1 < kGridRows) {
    // has bottom
    copy_colbuf_out(w, h, src, w, colBuf);
    MPI_Send(colBuf, h, MPI_FLOAT, kRank + kGridCols, tag, MPI_COMM_WORLD);
  }
  if (0 <= kGridY - 1) {
    // has left
    copy_rowbuf_out(w, h, src, 1, rowBuf);
    MPI_Send(rowBuf, w, MPI_FLOAT, kRank - 1, tag, MPI_COMM_WORLD);
  }
  if (kGridY + 1 < kGridCols) {
    // has right
    copy_rowbuf_out(w, h, src, h, rowBuf);
    MPI_Send(rowBuf, w, MPI_FLOAT, kRank + 1, tag, MPI_COMM_WORLD);
  }
}

void recv_ghosts(int tag, int w, int h, float dst[w + 2][h + 2],
                 float rowBuf[w], float colBuf[h]) {
  /* TODO:
   * Receive values from the appropriate neighbors into dst, 
   * using rowBuf and colBuf as the send buffers and tag as the
   * tag for the messages. You may use the provided copy buffer
   * helper functions for this task. Use the grid information
   * to determine which ranks to receive values from. 
   */
  if (0 <= kGridX - 1) {
    // has top
    MPI_Recv(colBuf, h, MPI_FLOAT, kRank - kGridCols, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    copy_colbuf_in(w, h, dst, 0, colBuf);
  }
  if (kGridX + 1 < kGridRows) {
    // has bottom
    MPI_Recv(colBuf, h, MPI_FLOAT, kRank + kGridCols, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    copy_colbuf_in(w, h, dst, w+1, colBuf);
  }
  if (0 <= kGridY - 1) {
    // has left
    MPI_Recv(rowBuf, w, MPI_FLOAT, kRank - 1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    copy_rowbuf_in(w, h, dst, 0, rowBuf);
  }
  if (kGridY + 1 < kGridCols) {
    // has right
    MPI_Recv(rowBuf, w, MPI_FLOAT, kRank + 1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    copy_rowbuf_in(w, h, dst, h+1, rowBuf);
  }
}

double get_wall_time() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return tp.tv_sec + (tp.tv_usec / 1e6);
}

int main(int argc, char *argv[]) {
  // Setup the environment
  setup_env(argc, argv);
#if VERBOSE_LEVEL >= 1
  // Print the grid information from PE0
  if (kRank == 0) {
    printf("[%d] there are %d pes, divided into a %d x %d grid.\n", kRank,
           kNumPes, kGridRows, kGridCols);
  }
#endif
#if VERBOSE_LEVEL >= 2
  printf("[%d] corresponds to coordinate (%d,%d).\n", kRank, kGridX, kGridY);
#endif
  // Compute the size of our local stencil
  int w = kWidth / kGridRows;
  int h = kHeight / kGridCols;
  assert(((w * kGridRows) == kWidth) && ((h * kGridCols) == kHeight));
#if VERBOSE_LEVEL >= 2
  // Print the tile size from PE0
  if (kRank == 0) {
    printf("[%d] each PE is taking a %d by %d tile.\n", kRank, w, h);
  }
#endif
  // Allocate all of the necessary buffers
  float *in = (float *)malloc(sizeof(float) * (w + 2) * (h + 2));
  float *out = (float *)malloc(sizeof(float) * (w + 2) * (h + 2));
  float *rowBuf = (float *)malloc(sizeof(float) * w);
  float *colBuf = (float *)malloc(sizeof(float) * h);
  // Initialize the stencils
  init_stencil(kGridX * w, kGridY * h, w, h, (float(*)[w + 2])in);
  memcpy(out, in, sizeof(float) * (w + 2) * (h + 2));
  double start, end;
  start = get_wall_time();
  // Iterate for the number of iterations
  int it;
  for (it = 0; it < kNumIts; it++) {
    // Send our ghost cells to the other PEs
    send_ghosts(it, w, h, (float(*)[w + 2])in, rowBuf, colBuf);
    // Receive ghost cells from the other PEs
    recv_ghosts(it, w, h, (float(*)[w + 2])in, rowBuf, colBuf);
    // Perform our local stencil computation
    stencil_2d(w, h, (float(*)[w + 2])in, (float(*)[w + 2])out);
    // Swap the in and out buffers for the next iteration
    swap(&in, &out);
  }
  // Wait for all of the threads to finish communicating
  MPI_Barrier(MPI_COMM_WORLD);
  end = get_wall_time();
  if (kRank == 0) {
    // Print the runtime from PE0
    printf("[%d] It took %.3lf seconds to run %d iterations.\n",
           kRank, end - start, kNumIts);
  }
  // Collect the results at the root
  if (kRank == 0) {
    float *all = (float *)malloc(sizeof(float) * kWidth * kHeight);
    float *recv = (float *)malloc(sizeof(float) * w * h);
    for (it = 0; it < kNumPes; it++) {
      int x = (it / kGridCols) * w;
      int y = (it % kGridCols) * h;
#if VERBOSE_LEVEL >= 2
      printf("[%d] Receiving values from %d into (%d, %d).\n", kRank, it, x, y);
#endif
      // No receive needed for PE0 since the tile is local
      if (it == 0) {
        copy_stencil_to_buffer(w, h, (float(*)[w + 2])in, (float(*)[w])recv);
      } else {
        MPI_Recv(recv, w * h, MPI_FLOAT, it, it, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
      }
      copy_buffer_to_stencil(kWidth, kHeight, (float(*)[kWidth])all, w, h,
                             (float(*)[w])recv, x, y);
    }
#if VERBOSE_LEVEL >= 1
    printf("[%d] Final results:\n", kRank);
    print_matrix(kWidth, kHeight, (float(*)[kWidth])all);
#endif
    free(all);
    free(recv);
  } else {
    float *send = (float *)malloc(sizeof(float) * w * h);
    copy_stencil_to_buffer(w, h, (float(*)[w + 2])in, (float(*)[w])send);
    MPI_Send(send, w * h, MPI_FLOAT, 0, kRank, MPI_COMM_WORLD);
    free(send);
  }
  // free the various buffers
  free(in);
  free(out);
  free(rowBuf);
  free(colBuf);
  // terminate normally
  MPI_Finalize();
  return 0;
}

void init_stencil(int xOff, int yOff, int m, int n, float a[m + 2][n + 2]) {
  int i, j;
  for (i = 0; i <= (m + 1); i++) {
    for (j = 0; j <= (n + 1); j++) {
      if (i == 0 || j == 0 || i == (m + 1) || j == (n + 1)) {
        a[i][j] = 0.0f;
      } else {
        a[i][j] = SCALING_FACTOR * ((i + xOff) * (j + yOff));
      }
    }
  }
}

void stencil_2d(int w, int h, float in[w + 2][h + 2], float out[w + 2][h + 2]) {
  int i, j;
  for (i = 1; i <= w; i++) {
    for (j = 1; j <= h; j++) {
      out[i][j] =
          in[i + 1][j] + in[i - 1][j] + in[i][j] + in[i][j - 1] + in[i][j + 1];
    }
  }
}

void copy_stencil_to_buffer(int w, int h, float in[w + 2][h + 2],
                            float out[w][h]) {
  int i;
  for (i = 0; i < w; i++) {
    memcpy(&out[i][0], &in[i + 1][1], sizeof(float) * h);
  }
}

void copy_buffer_to_stencil(int w, int h, float dst[w][h], int ww, int hh,
                            float src[ww][hh], int x, int y) {
  int i;
  for (i = 0; i < ww; i++) {
    memcpy(&dst[x + i][y], &src[i][0], sizeof(float) * hh);
  }
}

void print_matrix(int m, int n, float a[m][n]) {
  int i, j;
  for (i = 0; i < m; i++) {
    for (j = 0; j < n; j++) {
      printf("%.3f\t", a[i][j]);
    }
    printf("\n");
  }
}

void swap(float **a, float **b) {
  float *tmp = *a;
  *a = *b;
  *b = tmp;
}

// A function to find largest prime factor
// Adapted from: https://www.geeksforgeeks.org/find-largest-prime-factor-number/
int max_prime_factor(int n) {
  if (n == 1)
    return 1;
  int maxPrime = -1;
  while (n % 2 == 0) {
    maxPrime = 2;
    n >>= 1;
  }
  int i;
  for (i = 3; i <= sqrt(n); i += 2) {
    while (n % i == 0) {
      maxPrime = i;
      n = n / i;
    }
  }
  if (n > 2)
    maxPrime = n;
  return maxPrime;
}

int kRank, kNumPes, kNumIts;
int kGridRows, kGridCols;
int kGridX, kGridY;
int kWidth, kHeight;

void setup_env(int argc, char *argv[]) {
  // Initialize the MPI environment
  MPI_Init(&argc, &argv);
  // Get the rank of the process
  MPI_Comm_rank(MPI_COMM_WORLD, &kRank);
  // Get the number of processes
  MPI_Comm_size(MPI_COMM_WORLD, &kNumPes);
  // Error if unexpected number of args
  if (argc != 4) {
    if (kRank == 0) {
      fprintf(stderr, "usage is: %s <width> <height> <numIts>\n", argv[0]);
    }
    exit(1);
  }
  // Parse the arguments
  kWidth = atoi(argv[1]);
  kHeight = atoi(argv[2]);
  kNumIts = atoi(argv[3]);
  assert((kWidth > 0) && (kHeight > 0) && (kNumIts >= 0));
  // Establish the communications grid
  kGridCols = max_prime_factor(kNumPes);
  kGridRows = kNumPes / kGridCols;
  assert((kGridRows * kGridCols) == kNumPes);
  // Find our coordinates within the grid
  kGridX = kRank / kGridCols;
  kGridY = kRank % kGridCols;
}

void copy_rowbuf_out(int w, int h, float src[w + 2][h + 2], int y,
                     float rowBuf[w]) {
  int i;
  for (i = 1; i <= w; i++) {
    rowBuf[i - 1] = src[i][y];
  }
}

void copy_colbuf_out(int w, int h, float src[w + 2][h + 2], int x,
                     float colBuf[h]) {
  memcpy(colBuf, &src[x][1], sizeof(float) * h);
}

void copy_rowbuf_in(int w, int h, float dst[w + 2][h + 2], int y,
                    float rowBuf[w]) {
  int i;
  for (i = 1; i <= w; i++) {
    dst[i][y] = rowBuf[i - 1];
  }
}

void copy_colbuf_in(int w, int h, float dst[w + 2][h + 2], int x,
                    float colBuf[h]) {
  memcpy(&dst[x][1], colBuf, sizeof(float) * h);
}
