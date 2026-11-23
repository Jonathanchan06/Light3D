#include <stdio.h>
#include <string.h>

#include "image.h"
#include "test.h"

/* Writes `img` to a temporary file and reads it back into `buf`. */
static int ppm_to_string(const Image *img, char *buf, size_t size)
{
    FILE *f = tmpfile();
    if (f == NULL) {
        return -1;
    }
    int rc = image_write_ppm(img, f);
    rewind(f);
    size_t n = fread(buf, 1, size - 1, f);
    buf[n] = '\0';
    fclose(f);
    return rc;
}

static void test_create_rejects_bad_size(void)
{
    Image img;
    CHECK(image_create(&img, 0, 10) != 0);
    CHECK(image_create(&img, 10, -1) != 0);
}

static void test_create_starts_black(void)
{
    Image img;
    CHECK(image_create(&img, 3, 2) == 0);
    CHECK(img.width == 3);
    CHECK(img.height == 2);
    for (int i = 0; i < 3 * 2 * 3; i++) {
        CHECK(img.data[i] == 0.0);
    }
    image_free(&img);
}

static void test_write_ppm_format(void)
{
    Image img;
    CHECK(image_create(&img, 2, 1) == 0);
    image_set_pixel(&img, 0, 0, 1.0, 0.0, 0.5);
    image_set_pixel(&img, 1, 0, 0.0, 1.0, 0.0);

    char buf[256];
    CHECK(ppm_to_string(&img, buf, sizeof buf) == 0);
    CHECK(strcmp(buf, "P3\n2 1\n255\n255 0 128\n0 255 0\n") == 0);
    image_free(&img);
}

static void test_write_ppm_clamps(void)
{
    Image img;
    CHECK(image_create(&img, 1, 1) == 0);
    image_set_pixel(&img, 0, 0, -1.0, 7.0, NAN);

    char buf[256];
    CHECK(ppm_to_string(&img, buf, sizeof buf) == 0);
    CHECK(strcmp(buf, "P3\n1 1\n255\n0 255 0\n") == 0);
    image_free(&img);
}

int main(void)
{
    RUN_TEST(test_create_rejects_bad_size);
    RUN_TEST(test_create_starts_black);
    RUN_TEST(test_write_ppm_format);
    RUN_TEST(test_write_ppm_clamps);
    return TEST_SUMMARY();
}
