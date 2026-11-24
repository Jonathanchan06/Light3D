#ifndef RENDER_H
#define RENDER_H

#include "image.h"
#include "ray.h"

/* The main loop: for each pixel, get a ray from the camera, find its color,
 * and store it in the Image.
 *
 *   - Step 1: ray_color returns a sky gradient based on the ray direction.
 *     This replaces the x/y gradient currently in main.c.
 *   - Step 2: color by surface normal when a sphere is hit.
 *   - Step 3: several samples per pixel, recursive bounces with a max depth,
 *     gamma correction. See skills.md "Sampling and color".
 *   - Step 5: time the render. */

#endif
