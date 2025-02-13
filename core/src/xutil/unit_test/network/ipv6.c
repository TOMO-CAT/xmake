#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_network_ipv6_main(xu_int_t argc, xu_char_t** argv)
{
    // done
    xu_ipv6_t ipv6;
    if (xu_ipv6_cstr_set(&ipv6, argv[1]))
    {
        // trace
        xu_trace_i("%s => %{ipv6}", argv[1], &ipv6);
    }

    // end
    return 0;
}
