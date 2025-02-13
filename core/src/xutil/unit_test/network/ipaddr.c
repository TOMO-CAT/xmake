#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_network_ipaddr_main(xu_int_t argc, xu_char_t** argv)
{
    // done
    xu_ipaddr_t addr;
    xu_ipaddr_clear(&addr);
    if (xu_ipaddr_ip_cstr_set(&addr, argv[1], XU_IPADDR_FAMILY_NONE))
    {
        // trace
        xu_trace_i("%s => %{ipaddr}", argv[1], &addr);
    }
    return 0;
}
