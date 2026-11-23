# skills.md

Reference techniques for this codebase, with the pitfalls that commonly cause bugs. The **Stage 1** section is the current scope; later sections are for when you reach them in `roadmap.md`.

---

# Stage 1

## Images (PPM)
- Plain PPM (`P3`): header `P3\n<width> <height>\n255\n`, then one `r g b` line per pixel, top row first.
- Pixel values are stored as `double` in [0, 1]. Convert at write time: gamma-correct, clamp to [0, 0.999], multiply by 256, cast to `int`.

## Vectors and rays
- `Vec3` is a small struct of three `double`s, passed and returned by value. Put the functions in `vec3.h` as `static inline` so they're fast and need no `.c` file.
- A ray is `P(t) = origin + t·direction`. Only `t > 0` is in front of the ray.
- `normalize` divides by the length. Never normalize a zero vector (it produces NaN). Check with a small threshold first.

## Ray–sphere intersection
- Solve `|o + t·d − c|² = r²`. With `oc = o − c`:
  `a = dot(d, d)`, `h = dot(d, oc)`, `c = dot(oc, oc) − r²`, `disc = h² − a·c`.
  If `disc < 0`, miss. Otherwise roots are `t = (−h ± sqrt(disc)) / a`.
- Try the smaller root first; if it's outside `(t_min, t_max)`, try the larger one.
- Use `t_min = 0.001`. With `t_min = 0`, bounced rays hit the surface they started from due to rounding, causing dark speckles ("shadow acne").
- The outward normal is `(p − c) / r`. Store `front_face = dot(d, outward) < 0`, and flip the normal when false, so it always points against the ray.
- Closest hit in a list: loop over objects, and shrink `t_max` to the current closest `t` each time something is hit.

## Random numbers
- Use a small generator with explicit state, e.g. xorshift64* or PCG32. Never use `rand()`: it has hidden global state and poor quality.
- Seed each pixel sample from a hash of (pixel index, sample index), e.g. splitmix64. This makes output deterministic and later makes threading easy.
- `random_double(&rng)` returns [0, 1). Build `random_in_unit_sphere` by rejection sampling (pick points in the cube until one is inside the sphere), and `random_unit_vector` by normalizing that.

## Sampling and color
- Antialiasing: for each pixel, average several rays with random offsets inside the pixel.
- Recursion: `ray_color(ray, depth)` returns black when `depth` reaches 0, so bounces always terminate.
- Gamma 2: take `sqrt` of each channel before writing. Without it, images look too dark.

## Materials
- A material's `scatter` function takes the incoming ray and hit record, and returns whether the ray scatters, the scattered ray and an attenuation color.
- **Lambertian:** scatter toward `normal + random_unit_vector()`. If that's near zero in all components, use the normal instead (avoids NaN).
- **Metal:** `reflect(d, n) = d − 2·dot(d, n)·n`. Add `fuzz · random_in_unit_sphere()`. If the result points into the surface (`dot(scattered, n) <= 0`), absorb the ray.
- **Dielectric (glass):**
  - Refraction ratio is `1/η` when `front_face` is true (entering), and `η` when false (leaving).
  - Total internal reflection: if `ratio · sin θ > 1`, reflect instead of refracting.
  - Schlick approximation gives reflectance; reflect with that probability (`random_double() < reflectance`).
  - Use a unit-length incoming direction for the angle math.

## Camera
- Field of view: viewport height `= 2·tan(fov/2)`. Build an orthonormal basis `w = normalize(from − at)`, `u = normalize(cross(up, w))`, `v = cross(w, u)`.
- Defocus blur: start rays from a random point on a disk around the camera origin, aimed at the focus plane.

## C pitfalls to watch for
- Integer division: `1/2` is `0`. Write `1.0/2` or `0.5`.
- `abs()` is for `int`; use `fabs()` for `double`.
- Uninitialized struct fields: initialize with `= {0}` or designated initializers.
- Link the math library with `-lm`.
- If the sanitizer build reports an error, fix it before moving on, even if the image looks fine.

## Testing
- Compare doubles with a tolerance: `fabs(a − b) < 1e-9`, not `==`.
- Intersection tests: hit, miss, grazing (tangent) ray, ray starting inside, object behind the ray.

---

# Stage 2: Performance (later)

## Benchmarking
- Fix one benchmark setting and never change it. Compare only runs at identical settings.
- Build with `-O3 -march=native` for benchmarks. Record CPU and compiler version with every result.
- Profile before optimizing: `valgrind --tool=callgrind` or `gprof` work in WSL; `perf` is awkward there.

## Threading
- Divide the image into tiles (e.g. 32×32). Workers take the next tile index from an `atomic_int` (`<stdatomic.h>`) with `atomic_fetch_add`.
- Each worker owns its RNG state and scratch memory. Each tile writes a separate part of the framebuffer, so no locks are needed.
- Join all workers before reading the framebuffer.
- Run ThreadSanitizer (`-fsanitize=thread`) after any threading change. It can't be combined with AddressSanitizer, so it needs its own build.
- With per-pixel seeding, the multithreaded image must be byte-identical to the single-threaded one. This is a good test.

## Ray–triangle (Möller–Trumbore)
- `e1 = v1 − v0`, `e2 = v2 − v0`, `pvec = cross(d, e2)`, `det = dot(e1, pvec)`. Reject if `fabs(det) < 1e-8`.
- Use `fabs(det)`, not `det`, so back faces are hit too (needed for glass).
- Barycentrics must satisfy `u ≥ 0`, `v ≥ 0`, `u + v ≤ 1`.
- Precompute `e1`, `e2` when loading.
- Mesh scale: `t_min = 0.001` can be too large for tiny models. Normalize the mesh to roughly unit size when loading.

## BVH
- AABB slab test with precomputed `inv_d = 1/d`. Zero components become ±infinity, which works, except when the origin lies exactly on a slab plane: `0 · inf = NaN`. Use `fmin`/`fmax`, which ignore NaN, to combine slabs.
- Start with a simple midpoint split; move to binned SAH (12–16 bins on centroids) once it works.
- Degenerate case: if all centroids in a node are identical, make a leaf instead of splitting.
- With SAH, only split when the split cost is lower than the leaf cost. Leaves hold 2–4 primitives.
- Store nodes in one flat array (`BvhNode { aabb; left_or_first; count; }`), with children next to each other.
- Traversal: iterative with a fixed stack of 64 entries, visit the nearer child first, shrink `t_max` on each hit.
- Test against brute force on random rays: compare `t` exactly, but accept either primitive when two hit at the same `t`.
- Use an arena allocator for mesh and BVH data, freed in one call.

---

# Stage 3: Web (later)

## WebAssembly
- Export plain C functions with `EMSCRIPTEN_KEEPALIVE`. Keep renderer state behind an opaque handle pointer returned to JS, not a global.
- Pass the framebuffer to JS as a pointer and length, viewed through `HEAPU8`. With `-sALLOW_MEMORY_GROWTH=1`, re-create views after any allocation, because the buffer can be replaced.
- The canvas needs RGBA8. Keep a float accumulation buffer and convert it for display.
- Don't block the page: render in chunks (e.g. a few tiles per `requestAnimationFrame`), or in a Web Worker.
- **Threads (optional):** need SharedArrayBuffer, which needs `Cross-Origin-Opener-Policy: same-origin` and `Cross-Origin-Embedder-Policy: require-corp`. GitHub Pages can't set these.
  - A `-pthread` build won't load at all without them. Ship a separate single-threaded build and pick one in JS by checking `crossOriginIsolated`.
  - Never call `pthread_join` on the browser main thread. Use `-sPROXY_TO_PTHREAD` or start rendering from a dedicated pthread.

---

# Stage 4: Extras (later)

## SIMD
- Wasm SIMD (`-msimd128`, `wasm_simd128.h`) holds only 2 doubles per register, so gains on `double` code are small. A `float` path for the BVH slab test is the most promising target.
- Benchmark every SIMD change, and keep it only if it's faster.

## Image regression tests
- Build tests without `-march=native` and with `-ffp-contract=off`, so images match across machines.
- Compare against stored low-resolution references with a small tolerance. Update references only on purpose (`make update-refs`), with the reason in the commit message.
