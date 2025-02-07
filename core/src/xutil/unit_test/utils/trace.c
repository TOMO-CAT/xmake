#include "xutil/utils/trace.h"
#include "xutil/unit_test/ut.h"

/* *******************************************************
 *  main
 */
xu_int_t xu_ut_utils_trace_main(xu_int_t argc, xu_char_t** argv)
{
    xu_int_t   i = 10;
    xu_float_t f = 3.1415926;
    xu_char_t  c = 'A';

    {
        // xmake trace info
        xu_trace_i("[utils-trace]: int: %d float: %.2f char: %c", i, f, c);
        // xmake trace error (依赖 __xu_debug__ 宏)
        xu_trace_e("[utils-trace]: error msg");
        // xmake trace assert (依赖 __xu_debug__ 宏)
        xu_trace_a("[utils-trace] assert msg");
    }

    return 0;
}
