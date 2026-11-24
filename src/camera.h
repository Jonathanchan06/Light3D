#ifndef CAMERA_H
#define CAMERA_H

#include "ray.h"

/* Camera: turns a pixel position into a ray.
 *
 * Step 1 (simple camera):
 *   - Camera sits at the origin and looks down -z.
 *   - A "viewport" rectangle in front of it; each pixel maps to a point on it.
 *   - A function that, given pixel (x, y), returns the ray through that pixel.
 *   Decide: what does the Camera struct need to store so that function is cheap?
 *
 * Step 3: random offset inside the pixel (antialiasing).
 * Step 5: field of view, look-from / look-at, aspect ratio, optional defocus blur.
 *   See skills.md "Camera". */

#endif
