#include "xutil/unit_test/ut.h"

/* *******************************************************
 * test
 */
xu_void_t        xu_demo_test3(xu_noarg_t);
xu_void_t        xu_demo_test3() { xu_backtrace_dump("\t", xu_null, 10); }
static xu_void_t xu_demo_test2() { xu_demo_test3(); }
xu_void_t        xu_demo_test(xu_size_t size);
xu_void_t        xu_demo_test(xu_size_t size)
{
    if (size)
        xu_demo_test(size - 1);
    else
        xu_demo_test2();
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_platform_backtrace_main(xu_int_t argc, xu_char_t** argv)
{
    // done
    xu_demo_test(argv[1] ? xu_atoi(argv[1]) : 10);
    return 0;
}
