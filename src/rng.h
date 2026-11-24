#ifndef RNG_H
#define RNG_H

#include <stdint.h>

/* Step 3. A small seeded random number generator. See skills.md "Random numbers".
 *
 *   - An Rng struct holding the generator state (no hidden globals, unlike rand()).
 *   - Seeding from (pixel index, sample index), e.g. with splitmix64,
 *     so the same settings always give the same image.
 *   - random_double: a value in [0, 1).
 *   - random_in_unit_sphere, random_unit_vector (built on random_double).
 *
 * Like vec3.h, these are small enough to be `static inline` here. */

#endif
