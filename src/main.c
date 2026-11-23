#include <stdio.h>

#include "image.h"

int main(int argc, char **argv)
{
    const char *path = argc > 1 ? argv[1] : "out.ppm";
    const int width = 256;
    const int height = 144;

    Image img;
    if (image_create(&img, width, height) != 0) {
        fprintf(stderr, "error: could not allocate %dx%d image\n", width, height);
        return 1;
    }

    /* Gradient: red increases left to right, green increases top to bottom. */
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double r = (double)x / (width - 1);
            double g = (double)y / (height - 1);
            image_set_pixel(&img, x, y, r, g, 0.25);
        }
    }

    FILE *f = fopen(path, "w");
    if (f == NULL) {
        perror(path);
        image_free(&img);
        return 1;
    }
    int rc = image_write_ppm(&img, f);
    if (fclose(f) != 0) {
        rc = -1;
    }
    image_free(&img);

    if (rc != 0) {
        fprintf(stderr, "error: failed writing %s\n", path);
        return 1;
    }
    printf("wrote %s (%dx%d)\n", path, width, height);
    return 0;
}
