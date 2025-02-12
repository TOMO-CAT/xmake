#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_demo_network_ipv4_main(xu_int_t argc, xu_char_t** argv)
{
    // done
    xu_ipv4_t ipv4;
    if (xu_ipv4_cstr_set(&ipv4, argv[1]))
    {
        // trace
        xu_trace_i("%s => %{ipv4}", argv[1], &ipv4);
    }

    // end
    return 0;
}
