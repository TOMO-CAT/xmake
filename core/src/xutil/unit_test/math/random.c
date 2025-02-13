#include "xutil/unit_test/ut.h"

/* *******************************************************
 * test
 */
static xu_void_t xu_random_test(xu_long_t b, xu_long_t e)
{
    // init
    __xu_volatile__ xu_size_t   i    = 0;
    __xu_volatile__ xu_sint32_t n    = 1000000;
    __xu_volatile__ xu_long_t   rand = 0;

    // done
    xu_hong_t t = xu_mclock();
    for (i = 0; i < n; i++)
        rand += xu_random_range(b, e);
    t = xu_mclock() - t;

    // trace
    xu_trace_i("time: %lld, average: %d, range: %ld - %ld", t, (rand + (n >> 1)) / n, b, e);
}
#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
static xu_void_t xu_random_test_float(xu_float_t b, xu_float_t e)
{
    // init
    __xu_volatile__ xu_size_t  i    = 0;
    __xu_volatile__ xu_size_t  n    = 1000000;
    __xu_volatile__ xu_float_t rand = 0;

    // done
    xu_hong_t t = xu_mclock();
    for (i = 0; i < n; i++)
        rand += xu_random_rangef(b, e);
    t = xu_mclock() - t;

    // trace
    xu_trace_i("time: %lld, average: %f, range: %f - %f", t, rand / n, b, e);
}
#endif

/* *******************************************************
 * main
 */
xu_int_t xu_ut_math_random_main(xu_int_t argc, xu_char_t** argv)
{
    xu_random_test(600, 1000);
    xu_random_test(100, 200);
    xu_random_test(-600, 1000);
    xu_random_test(-600, 200);
    xu_random_test(-600, -200);

#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
    xu_random_test_float(0., 1.);
    xu_random_test_float(0., 200.);
    xu_random_test_float(-200., 0.);
    xu_random_test_float(-200., 200.);
#endif

    return 0;
}
