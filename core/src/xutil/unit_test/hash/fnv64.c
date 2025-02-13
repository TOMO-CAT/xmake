#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_hash_fnv64_main(xu_int_t argc, xu_char_t** argv)
{
    // trace
    xu_trace_i("[fnv64]:    %llx", xu_fnv64_make_from_cstr(argv[1], 0));
    xu_trace_i("[fnv64-1a]: %llx", xu_fnv64_1a_make_from_cstr(argv[1], 0));
    return 0;
}
