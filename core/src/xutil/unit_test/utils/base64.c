#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_utils_base64_main(xu_int_t argc, xu_char_t** argv)
{
    xu_char_t ob[4096] = {0};
    xu_size_t on       = xu_base64_encode((xu_byte_t*)argv[1], xu_strlen(argv[1]), ob, 4096);
    // xu_size_t on = xu_base64_decode((xu_byte_t*)argv[1], xu_strlen(argv[1]), ob, 4096);
    xu_printf("%s: %lu\n", ob, on);

    return 0;
}
