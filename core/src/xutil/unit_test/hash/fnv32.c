#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_hash_fnv32_main(xu_int_t argc, xu_char_t** argv)
{
    // trace
    xu_trace_i("[fnv32]:    %x", xu_fnv32_make_from_cstr(argv[1], 0));
    xu_trace_i("[fnv32-1a]: %x", xu_fnv32_1a_make_from_cstr(argv[1], 0));
    return 0;
}
