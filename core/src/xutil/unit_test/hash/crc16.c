#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_hash_crc16_main(xu_int_t argc, xu_char_t** argv)
{
    xu_trace_i("[crc16_ansi]:   %x\n", xu_crc16_make_from_cstr(argv[1], 0));
    xu_trace_i("[crc16_ccitt]:  %x\n", xu_crc16_ccitt_make_from_cstr(argv[1], 0));
    return 0;
}
