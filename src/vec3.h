#ifndef VEC3_H
#define VEC3_H

#include <math.h>

/* Step 1. A point, direction or color in 3D. See docs/vec3.md and
 * skills.md "Vectors and rays".
 *
 * Every function takes and returns Vec3 by value, and is `static inline`
 * so it can live in this header with no vec3.c. */

typedef struct {
    double x, y, z;
} Vec3;

static inline Vec3 vec3_add(Vec3 a, Vec3 b)
{
    return (Vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

/* a - b: the arrow from b to a. */
static inline Vec3 vec3_sub(Vec3 a, Vec3 b)
{
    return (Vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

static inline Vec3 vec3_scale(Vec3 v, double t)
{
    return (Vec3){v.x * t, v.y * t, v.z * t};
}

static inline double vec3_dot(Vec3 a, Vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

/* A vector perpendicular to both a and b. Order matters: cross(b, a) = -cross(a, b). */
static inline Vec3 vec3_cross(Vec3 a, Vec3 b)
{
    return (Vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
    };
}

static inline double vec3_length_squared(Vec3 v)
{
    return vec3_dot(v, v);
}

static inline double vec3_length(Vec3 v)
{
    return sqrt(vec3_length_squared(v));
}

/* Returns v scaled to length 1.
 * If v is (nearly) zero, there is no direction to keep, so this returns
 * (0,0,0) instead of dividing by zero (which would give NaN). Callers that
 * need a real direction must check for that case before normalizing. */
static inline Vec3 vec3_normalize(Vec3 v)
{
    double len2 = vec3_length_squared(v);
    if (len2 < 1e-16) { /* length < 1e-8 */
        return (Vec3){0, 0, 0};
    }
    return vec3_scale(v, 1.0 / sqrt(len2));
}

#endif
