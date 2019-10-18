#include "mandelbrot.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <complex.h>
#include <png.h>
#include <unistd.h>
#include <pthread.h>

const int RE_START = -2;
const int RE_END = 1;
const int IM_START = -1;
const int IM_END = 1;
const int MAX_ITER = 80;

mandelbrot_t kData;

int main(int argc, char** argv)
{
    // Initialize default values
    char* dest = "out.png";
    int serial = 1;
    kData.width = 480;
    kData.height = 480;
    kData.numThreads = 1;
    // Parse the arguments
    int c;
    while ((c = getopt(argc, argv, "w:h:o:n:")) != -1) {
        switch (c) {
        case 'w':
            kData.width = atoi(optarg);
            break;
        case 'h':
            kData.height = atoi(optarg);
            break;
        case 'o':
            dest = optarg;
            break;
        case 'n':
            serial = 0;
            kData.numThreads = atoi(optarg);
            break;
        default:
            abort();
        }
    }
    // Allocate the image buffer
    kData.img = malloc(sizeof(int) * kData.width * kData.height * 3);
    // Directly call `generateMandelbrot' in serial mode
    if (serial) {
        arg_t args;
        args.startX = 0;
        args.endX = kData.width;
        args.width = kData.width;
        args.height = kData.height;
        generateMandelbrot(&args);
    }
    else {
        // TODO fork-join into (kData.numThreads) threads
        // each task should call generateMandelbrot with an appropriate, non-overlapping startX and endX as follows:
        //   generateMandelbrot(startX, endX, kData.width, kData.height, (int(*)[(kData.height)][3])kData.img);
        // it is up to you to decide how to divide the work between threads
        int offset = 0;
        int interval = (int) kData.width / kData.numThreads;
        pthread_t threads[kData.numThreads];
        arg_t args[kData.numThreads];
        for (int i = 0; i < kData.numThreads - 1; i++) {
            args[i].startX = offset;
            args[i].endX = offset + interval;
            args[i].width = kData.width;
            args[i].height = kData.height;
            pthread_create(&threads[i], NULL, generateMandelbrot, &args[i]);
            offset += interval;
        }
        args[kData.numThreads-1].startX = offset;
        args[kData.numThreads-1].endX = kData.width;
        args[kData.numThreads-1].width = kData.width;
        args[kData.numThreads-1].height = kData.height;
        pthread_create(&threads[kData.numThreads-1], NULL, generateMandelbrot, &args[kData.numThreads-1]);
        
        for (int i = 0; i < kData.numThreads; i++)
            pthread_join(threads[i], NULL);
    }
    // Write the output file 
    int rc = writeMandelbrot(dest, kData.width, kData.height, (int(*)[kData.height][3])kData.img);
    // Free the image buffer
    free(kData.img);
    return rc;
}

inline int mandelbrot(int x, int y, int width, int height)
{
    complex double c = (RE_START + (((double)x) / width) * (RE_END - RE_START)) + 
                       (IM_START + (((double)y) / height) * (IM_END - IM_START)) * I;
    complex double z = 0;
    int n = 0;
    while ((cabs(z) <= 2) && (n < MAX_ITER)) {
        z = z * z + c;
        n++;
    }
    return 255 - (int)(((double)(n * 255)) / MAX_ITER);
}

void generateMandelbrot(arg_t* args)
{
    int (*img)[args->height][3] = (int(*)[kData.height][3])kData.img;
    // Assign the result of mandelbrot(x,y) to each value in our section
    for (int x = args->startX; x < args->endX; x++) {
        for (int y = 0; y < args->height; y++) {
            int cval = mandelbrot(x, y, args->width, args->height);
            img[x][y][0] = cval;
            img[x][y][1] = cval;
            img[x][y][2] = cval;
        }
    }
}

int writeMandelbrot(const char* dest, int width, int height, int img[width][height][3])
{
    int code = 0;
    FILE* fp = NULL;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_bytep row = NULL;

    // Open file for writing (binary mode)
    fp = fopen(dest, "wb");
    if (fp == NULL) {
        fprintf(stderr, "Could not open file %s for writing\n", dest);
        code = 1;
        goto finalise;
    }

    // Initialize write structure
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        fprintf(stderr, "Could not allocate write struct\n");
        code = 1;
        goto finalise;
    }

    // Initialize info structure
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fprintf(stderr, "Could not allocate info struct\n");
        code = 1;
        goto finalise;
    }

    // Setup Exception handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Error during png creation\n");
        code = 1;
        goto finalise;
    }

    png_init_io(png_ptr, fp);

    // Write header (8 bit colour depth)
    png_set_IHDR(png_ptr, info_ptr, width, height,
        8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    // Allocate memory for one row (3 bytes per pixel - RGB)
    row = (png_bytep)malloc(3 * width * sizeof(png_byte));

    // Write image data
    int x, y;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            (&(row[x * 3]))[0] = img[x][y][0];
            (&(row[x * 3]))[1] = img[x][y][1];
            (&(row[x * 3]))[2] = img[x][y][2];
        }
        png_write_row(png_ptr, row);
    }

    // End write
    png_write_end(png_ptr, NULL);

finalise:
    if (fp != NULL)
        fclose(fp);
    if (info_ptr != NULL)
        png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if (png_ptr != NULL)
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    if (row != NULL)
        free(row);

    return code;
}
