#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_hash_md5_main(xu_int_t argc, xu_char_t** argv)
{
    xu_byte_t ob[16];
    xu_size_t on = xu_md5_make((xu_byte_t const*)argv[1], xu_strlen(argv[1]), ob, 16);
    if (on != 16) return 0;

    xu_size_t i        = 0;
    xu_char_t md5[256] = {0};
    for (i = 0; i < 16; ++i)
        xu_snprintf(md5 + (i << 1), 3, "%02x", ob[i]);
    xu_printf("%s: %lu\n", md5, on);

    return 0;
}
