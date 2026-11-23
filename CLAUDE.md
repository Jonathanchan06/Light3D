# CLAUDE.md

## Project
A ray tracer written in C11, built as a **learning project**. The owner knows basic C and is new to ray tracing, performance work and multithreading.

Current scope (Stage 1): a native, single-threaded ray tracer that renders a scene to a PPM image. Later stages (performance, threads, web) are in `roadmap.md` and start only when the owner chooses.

Priorities, in order: understanding, correctness, simplicity. Performance comes later.

## How to work with the owner
1. **Explain first, then let the owner write the core code.** For ray tracing logic (vectors, intersections, materials, camera), explain the idea and the math, point to the relevant part of `skills.md`, and let the owner implement it. Review their code and help debug.
2. **Write plumbing directly.** Makefile, test runner, PPM writer boilerplate, `.gitignore` and similar setup can be written in full.
3. **Write core code only when asked.** When you do, keep it short, explain each part, and don't use techniques the owner hasn't met yet without saying what they are.
4. **Teach C along the way.** When something C-specific comes up (pointers, header guards, `static inline`, undefined behavior, sanitizer output), explain it briefly.
5. **Follow `roadmap.md` in order.** Don't introduce later-stage features (threads, BVH, meshes, wasm) early.
6. **Verify before reporting done:** `make && make test && make debug-test` must pass. Show the output summary.
7. **Keep changes small.** One concern per change. Don't refactor unrelated code.
8. **No dependencies** beyond the C standard library and `<math.h>`. Build with Make.
9. **Don't commit** unless the owner asks.

## Environment
- Build and run in **WSL Ubuntu** (`sudo apt install build-essential`). Sanitizers don't work with MinGW on Windows.
- View PPM output with an image viewer that supports PPM (e.g. GIMP, IrfanView), or convert it to PNG.

## Layout (Stage 1)
```
src/
  vec3.h        vector math (static inline functions)
  ray.h
  rng.h         small seeded random number generator
  camera.c/.h
  sphere.c/.h   sphere intersection, hit records
  material.c/.h
  scene.c/.h    builds the test scene
  render.c/.h   loops over pixels and samples
  image.c/.h    Image buffer and PPM writer
  main.c        CLI: render → PPM file
tests/
  test.h        minimal test runner (CHECK, CHECK_NEAR, RUN_TEST)
  test_*.c      each file is its own test program
  test_image.c
  test_vec3.c
  test_sphere.c
Makefile
```
Later stages add files; see `roadmap.md`.

## Commands
```
make             # optimized build → build/rt
make run         # render the default scene → out.ppm
make debug       # -O0 -g -fsanitize=address,undefined
make test        # unit tests
make debug-test  # unit tests under sanitizers
```

## Conventions
- C11. Flags: `-Wall -Wextra -Wpedantic -Werror`. Never use `-ffast-math`.
- Use `double` throughout.
- No global mutable state. Pass structs explicitly. This keeps code easy to test now and makes threads possible later.
- Names: `snake_case` functions, `PascalCase` typedefs (`Vec3`, `Ray`, `HitRecord`).
- Functions that can fail (file I/O, allocation) return an error code. Don't call `exit()` inside `src/` except in `main.c`.
- Every `malloc` has one clear owner and a matching `free`.
- Deterministic output: seed the RNG from (pixel index, sample index), so the same settings always produce the same image.
