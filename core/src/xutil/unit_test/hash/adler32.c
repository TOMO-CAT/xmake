#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_hash_adler32_main(xu_int_t argc, xu_char_t** argv)
{
    // trace
    xu_trace_i("[adler32]: %x", xu_adler32_make_from_cstr(argv[1], 0));
    return 0;
}
