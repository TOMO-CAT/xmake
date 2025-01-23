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
    xu_trace_i("[trace-ut]: int: %d float: %.2f char: %c", i, f, c);

    return 0;
}
