#include "ray.h"
#include "test.h"
#include "vec3.h"

/* Vec3 and Ray tests. Doubles are compared with CHECK_NEAR, not ==.
 * Inputs avoid lots of zeros and ones where possible: a term multiplied by
 * zero can't show a mistake. */

#define EPS 1e-9

static void test_add(void)
{
    Vec3 r = vec3_add((Vec3){1, 2, 3}, (Vec3){4, -5, 6});
    CHECK_NEAR(r.x, 5, EPS);
    CHECK_NEAR(r.y, -3, EPS);
    CHECK_NEAR(r.z, 9, EPS);
}

static void test_sub(void)
{
    /* sub is not symmetric: a - b and b - a point opposite ways. */
    Vec3 r = vec3_sub((Vec3){1, 2, 3}, (Vec3){4, 6, 8});
    CHECK_NEAR(r.x, -3, EPS);
    CHECK_NEAR(r.y, -4, EPS);
    CHECK_NEAR(r.z, -5, EPS);
}

static void test_scale(void)
{
    Vec3 r = vec3_scale((Vec3){1, 2, 3}, 2.0);
    CHECK_NEAR(r.x, 2, EPS);
    CHECK_NEAR(r.y, 4, EPS);
    CHECK_NEAR(r.z, 6, EPS);

    /* A negative t flips the direction. */
    r = vec3_scale((Vec3){1, 2, 3}, -1.0);
    CHECK_NEAR(r.x, -1, EPS);
    CHECK_NEAR(r.y, -2, EPS);
    CHECK_NEAR(r.z, -3, EPS);
}

static void test_dot(void)
{
    CHECK_NEAR(vec3_dot((Vec3){1, 2, 3}, (Vec3){4, 5, 6}), 32, EPS);
    /* Perpendicular vectors give 0. */
    CHECK_NEAR(vec3_dot((Vec3){1, 0, 0}, (Vec3){0, 1, 0}), 0, EPS);
    /* Opposite vectors give a negative number. */
    CHECK_NEAR(vec3_dot((Vec3){1, 2, 3}, (Vec3){-1, -2, -3}), -14, EPS);
}

static void test_cross(void)
{
    /* No zeros in the inputs, so every term of every component is exercised. */
    Vec3 a = {1, 2, 3};
    Vec3 b = {4, 5, 6};
    Vec3 r = vec3_cross(a, b);
    CHECK_NEAR(r.x, -3, EPS);
    CHECK_NEAR(r.y, 6, EPS);
    CHECK_NEAR(r.z, -3, EPS);

    /* The result is perpendicular to both inputs. */
    CHECK_NEAR(vec3_dot(r, a), 0, EPS);
    CHECK_NEAR(vec3_dot(r, b), 0, EPS);

    /* Swapping the order flips the sign. */
    Vec3 s = vec3_cross(b, a);
    CHECK_NEAR(s.x, 3, EPS);
    CHECK_NEAR(s.y, -6, EPS);
    CHECK_NEAR(s.z, 3, EPS);

    /* x cross y = z (right-handed axes). */
    Vec3 z = vec3_cross((Vec3){1, 0, 0}, (Vec3){0, 1, 0});
    CHECK_NEAR(z.x, 0, EPS);
    CHECK_NEAR(z.y, 0, EPS);
    CHECK_NEAR(z.z, 1, EPS);
}

static void test_length_squared(void)
{
    CHECK_NEAR(vec3_length_squared((Vec3){1, -2, 3}), 14, EPS);
    CHECK_NEAR(vec3_length_squared((Vec3){0, 0, 0}), 0, EPS);
}

static void test_length(void)
{
    CHECK_NEAR(vec3_length((Vec3){3, 4, 0}), 5, EPS);
    CHECK_NEAR(vec3_length((Vec3){2, -3, 6}), 7, EPS);
}

static void test_normalize(void)
{
    /* Length 1, same direction. */
    Vec3 r = vec3_normalize((Vec3){3, 4, 0});
    CHECK_NEAR(r.x, 0.6, EPS);
    CHECK_NEAR(r.y, 0.8, EPS);
    CHECK_NEAR(r.z, 0, EPS);
    CHECK_NEAR(vec3_length(r), 1, EPS);

    /* Zero vector: returns (0,0,0) instead of NaN. */
    r = vec3_normalize((Vec3){0, 0, 0});
    CHECK(r.x == 0 && r.y == 0 && r.z == 0);

    /* Tiny but valid vector: just above the threshold, still normalized. */
    r = vec3_normalize((Vec3){1e-6, 0, 0});
    CHECK_NEAR(r.x, 1, EPS);
}

static void test_ray_at(void)
{
    Ray ray = {{1, 0, 0}, {0, 0, -2}};

    Vec3 p = ray_at(ray, 1.5);
    CHECK_NEAR(p.x, 1, EPS);
    CHECK_NEAR(p.y, 0, EPS);
    CHECK_NEAR(p.z, -3, EPS);

    /* t = 0 is the origin. */
    p = ray_at(ray, 0);
    CHECK_NEAR(p.x, 1, EPS);
    CHECK_NEAR(p.y, 0, EPS);
    CHECK_NEAR(p.z, 0, EPS);
}

int main(void)
{
    RUN_TEST(test_add);
    RUN_TEST(test_sub);
    RUN_TEST(test_scale);
    RUN_TEST(test_dot);
    RUN_TEST(test_cross);
    RUN_TEST(test_length_squared);
    RUN_TEST(test_length);
    RUN_TEST(test_normalize);
    RUN_TEST(test_ray_at);
    return TEST_SUMMARY();
}
