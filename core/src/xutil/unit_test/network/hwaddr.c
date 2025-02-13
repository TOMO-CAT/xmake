#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_network_hwaddr_main(xu_int_t argc, xu_char_t** argv)
{
    // done
    xu_hwaddr_t addr;
    xu_hwaddr_clear(&addr);
    if (xu_hwaddr_cstr_set(&addr, argv[1]))
    {
        // trace
        xu_trace_i("%s => %{hwaddr}", argv[1], &addr);
    }
    return 0;
}
