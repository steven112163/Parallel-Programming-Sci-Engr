#ifndef MANDELBROT_H
#define MANDELBROT_H

extern const int RE_START;
extern const int RE_END;
extern const int IM_START;
extern const int IM_END;
extern const int MAX_ITER;

typedef struct mandelbrot {
    int width;
    int height;
    int numThreads;
    int *img;
} mandelbrot_t;

int mandelbrot(int x, int y, int width, int height);
void generateMandelbrot(int startX, int endX, int width, int height, int img[width][height][3]);
int writeMandelbrot(const char* dest, int width, int height, int img[width][height][3]);

#endif
