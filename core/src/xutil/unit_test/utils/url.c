#include "xutil/unit_test/ut.h"

/* xu_option_item_t
 * main
 */
xu_int_t xu_ut_utils_url_main(xu_int_t argc, xu_char_t** argv)
{
    xu_char_t ob[4096];
    // xu_size_t on = xu_url_encode(argv[1], xu_strlen(argv[1]), ob, 4096);
    xu_size_t on = xu_url_decode(argv[1], xu_strlen(argv[1]), ob, 4096);
    xu_trace_i("%s: %lu", ob, on);

    return 0;
}
