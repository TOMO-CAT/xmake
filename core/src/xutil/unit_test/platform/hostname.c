#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_platform_hostname_main(xu_int_t argc, xu_char_t** argv)
{
    xu_char_t name[256] = {0};
    if (xu_hostname(name, sizeof(name)))
    {
        xu_trace_i("hostname: %s", name);
    }
    return 0;
}
