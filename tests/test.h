#ifndef TEST_H
#define TEST_H

/* Minimal test runner. Each tests/test_*.c file is its own program:
 *
 *   static void test_something(void) { CHECK(1 + 1 == 2); }
 *
 *   int main(void) {
 *       RUN_TEST(test_something);
 *       return TEST_SUMMARY();
 *   }
 *
 * A failed CHECK prints the file, line and expression, then the test keeps
 * going so you see every failure at once. */

#include <math.h>
#include <stdio.h>

/* Counters are `static`, so each test program gets its own copy. */
static int test_checks_run = 0;
static int test_checks_failed = 0;

#define CHECK(cond)                                                              \
    do {                                                                         \
        test_checks_run++;                                                       \
        if (!(cond)) {                                                           \
            test_checks_failed++;                                                \
            fprintf(stderr, "%s:%d: CHECK failed: %s\n", __FILE__, __LINE__,     \
                    #cond);                                                      \
        }                                                                        \
    } while (0)

/* Doubles rarely compare equal exactly; check they're within `eps`. */
#define CHECK_NEAR(a, b, eps)                                                    \
    do {                                                                         \
        double check_a_ = (a), check_b_ = (b);                                   \
        test_checks_run++;                                                       \
        if (!(fabs(check_a_ - check_b_) <= (eps))) {                             \
            test_checks_failed++;                                                \
            fprintf(stderr, "%s:%d: CHECK_NEAR failed: %s = %g, %s = %g\n",      \
                    __FILE__, __LINE__, #a, check_a_, #b, check_b_);             \
        }                                                                        \
    } while (0)

#define RUN_TEST(fn)                                                             \
    do {                                                                         \
        int failed_before_ = test_checks_failed;                                 \
        fn();                                                                    \
        printf("  %-40s %s\n", #fn,                                              \
               test_checks_failed == failed_before_ ? "ok" : "FAILED");          \
    } while (0)

/* Prints a summary line and returns the exit code for main(). */
#define TEST_SUMMARY()                                                           \
    (printf("%s: %d checks, %d failed\n", __FILE__, test_checks_run,             \
            test_checks_failed),                                                 \
     test_checks_failed == 0 ? 0 : 1)

#endif
