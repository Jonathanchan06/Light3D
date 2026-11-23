#include "image.h"

#include <stdlib.h>

int image_create(Image *img, int width, int height)
{
    if (width <= 0 || height <= 0) {
        return -1;
    }
    size_t count = (size_t)width * (size_t)height * 3;
    double *data = calloc(count, sizeof *data); /* calloc zeroes memory: black image */
    if (data == NULL) {
        return -1;
    }
    img->width = width;
    img->height = height;
    img->data = data;
    return 0;
}

void image_free(Image *img)
{
    free(img->data);
    img->data = NULL;
    img->width = 0;
    img->height = 0;
}

void image_set_pixel(Image *img, int x, int y, double r, double g, double b)
{
    double *p = &img->data[((size_t)y * (size_t)img->width + (size_t)x) * 3];
    p[0] = r;
    p[1] = g;
    p[2] = b;
}

/* Maps [0, 1] to [0, 255]. Clamping to 0.999 keeps 1.0 from becoming 256.
 * `!(v > 0.0)` is also true for NaN, so a NaN pixel becomes 0 instead of
 * reaching the (int) cast, where converting NaN is undefined behavior. */
static int channel_to_byte(double v)
{
    if (!(v > 0.0)) {
        v = 0.0;
    }
    if (v > 0.999) {
        v = 0.999;
    }
    return (int)(256.0 * v);
}

int image_write_ppm(const Image *img, FILE *out)
{
    if (fprintf(out, "P3\n%d %d\n255\n", img->width, img->height) < 0) {
        return -1;
    }
    size_t count = (size_t)img->width * (size_t)img->height;
    for (size_t i = 0; i < count; i++) {
        const double *p = &img->data[i * 3];
        if (fprintf(out, "%d %d %d\n", channel_to_byte(p[0]), channel_to_byte(p[1]),
                    channel_to_byte(p[2])) < 0) {
            return -1;
        }
    }
    return 0;
}
