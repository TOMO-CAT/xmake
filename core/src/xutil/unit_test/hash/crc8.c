#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_hash_crc8_main(xu_int_t argc, xu_char_t** argv)
{
    xu_trace_i("[crc8]: %x\n", xu_crc8_make_from_cstr(argv[1], 0));
    return 0;
}
