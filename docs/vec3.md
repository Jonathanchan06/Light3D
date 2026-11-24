# Vector math notes (Step 1)

Reference: *Ray Tracing in One Weekend*, https://raytracing.github.io/books/RayTracingInOneWeekend.html

## Part 1: vectors, add/subtract/scale, length/normalize

### What a Vec3 is
Three doubles `(x, y, z)`. The same struct is used for:
- **points** (camera position, sphere center, hit point)
- **directions** (ray direction, surface normal)
- **colors** (r, g, b)

The math doesn't distinguish points from directions. You have to keep track.

Axes: x right, y up, z toward the viewer. The camera looks down -z.

### Add, subtract, scale (component by component)
```
add:      (a.x + b.x, a.y + b.y, a.z + b.z)
subtract: (a.x - b.x, a.y - b.y, a.z - b.z)
scale:    (a.x * t,   a.y * t,   a.z * t)
```
Meaning:
- point + direction = a new point (follow the arrow)
- point B - point A = the arrow from A to B (e.g. `hit_point - center` gives the sphere normal)
- direction * t: same line, different length; negative t flips it

Ray: `P(t) = origin + t * direction`. Only `t > 0` is in front of the ray. This is `ray_at(r, t)`.

### Length and normalize
```
length(v)         = sqrt(x² + y² + z²)
length_squared(v) = x² + y² + z²      (no sqrt; good for comparisons)
normalize(v)      = v / length(v)      (same direction, length 1)
```
Unit vectors matter wherever only direction counts (normals, lighting, reflection).

**Pitfall:** normalizing a zero vector divides by zero and gives NaN. Check for a near-zero
length first.

Example: origin `(1,0,0)`, direction `(0,0,-2)`. At `t = 1.5` the point is `(1,0,-3)`. With the
direction normalized to `(0,0,-1)`, it's `(1,0,-1.5)`. **`t` is a true distance only when the
direction has length 1.**

## Part 2: dot and cross

### Dot product
```
dot(a, b) = a.x*b.x + a.y*b.y + a.z*b.z      (a number)
          = |a| |b| cos(angle)
```
For unit vectors: 1 = same direction, 0 = perpendicular, -1 = opposite.

Uses: `length_squared(v) = dot(v, v)`; front/back test `dot(ray_dir, normal) < 0`;
sphere intersection terms; `reflect(d, n) = d - 2*dot(d, n)*n`.

### Cross product
```
cross(a, b) = (a.y*b.z - a.z*b.y,
               a.z*b.x - a.x*b.z,
               a.x*b.y - a.y*b.x)            (a vector)
```
The result is perpendicular to both a and b. Order matters: `cross(b, a) = -cross(a, b)`.
Right-hand rule: fingers along a, curl toward b, thumb = result.
`cross((1,0,0), (0,1,0)) = (0,0,1)`. Used for the camera basis (Step 5) and triangles (Stage 2).

## Part 3: writing it in C
- Pass and return `Vec3` **by value** (24 bytes, cheap). No pointers, no aliasing; code reads
  like the math.
- Build values with a **compound literal**: `return (Vec3){ x, y, z };`
- Define functions in `vec3.h` as **`static inline`**:
  - `inline`: the compiler may paste the body at the call site (no call overhead).
  - `static`: each `.c` file gets a private copy, so there's no "multiple definition" link error.
  - No `vec3.c` is needed.
- `sqrt` needs `<math.h>` and linking with `-lm`.
