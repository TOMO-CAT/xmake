#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_hash_djb2_main(xu_int_t argc, xu_char_t** argv)
{
    // trace
    xu_trace_i("[djb2]: %lx", xu_djb2_make_from_cstr(argv[1], 0));
    return 0;
}
