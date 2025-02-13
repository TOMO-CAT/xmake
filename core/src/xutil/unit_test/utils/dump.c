#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_utils_dump_main(xu_int_t argc, xu_char_t** argv)
{
    // dump
    xu_dump_data((xu_byte_t const*)argv[1], xu_strlen(argv[1]));
    return 0;
}
