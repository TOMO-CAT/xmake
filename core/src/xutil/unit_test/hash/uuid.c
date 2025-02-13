#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_hash_uuid_main(xu_int_t argc, xu_char_t** argv)
{
    // trace
    xu_char_t uuid[37];
    xu_trace_i("[uuid]: %s", xu_uuid_make_cstr(uuid, argv[1]));
    return 0;
}
