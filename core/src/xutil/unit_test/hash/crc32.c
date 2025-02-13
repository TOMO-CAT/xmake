#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_hash_crc32_main(xu_int_t argc, xu_char_t** argv)
{
    xu_trace_i("[crc32_ieee]:       %x\n", xu_crc32_make_from_cstr(argv[1], 0));
    xu_trace_i("[crc32_ieee_le]:    %x\n", xu_crc32_le_make_from_cstr(argv[1], 0));
    return 0;
}
