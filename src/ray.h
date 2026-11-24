#ifndef RAY_H
#define RAY_H

#include "vec3.h"

/* Step 1. A ray: P(t) = origin + t * direction. Only t > 0 is in front. */

typedef struct {
    Vec3 origin;
    Vec3 direction;
} Ray;

static inline Vec3 ray_at(Ray r, double t)
{
    return vec3_add(r.origin, vec3_scale(r.direction, t));
}

#endif
