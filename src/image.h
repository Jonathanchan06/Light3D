#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>

/* An RGB image stored as doubles, 3 per pixel, row by row from the top.
 * Channel values are nominally in [0, 1]; they're clamped when written. */
typedef struct {
    int width;
    int height;
    double *data; /* width * height * 3 values, owned by the Image */
} Image;

/* Allocates a black image. Returns 0 on success, -1 on bad size or out of memory. */
int image_create(Image *img, int width, int height);

/* Frees the pixel data. Safe to call twice. */
void image_free(Image *img);

void image_set_pixel(Image *img, int x, int y, double r, double g, double b);

/* Writes the image as plain-text PPM (P3). Returns 0 on success, -1 on write error. */
int image_write_ppm(const Image *img, FILE *out);

#endif
