#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_platform_utils_main(xu_int_t argc, xu_char_t** argv)
{
    // hostname
    xu_char_t hostname[4096] = {0};
    if (xu_hostname(hostname, 4096)) xu_trace_i("hostname: %s", hostname);
    return 0;
}
