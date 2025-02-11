#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_hash_sdbm_main(xu_int_t argc, xu_char_t** argv)
{
    // trace
    xu_trace_i("[sdbm]: %lx", xu_sdbm_make_from_cstr(argv[1], 0));
    return 0;
}
