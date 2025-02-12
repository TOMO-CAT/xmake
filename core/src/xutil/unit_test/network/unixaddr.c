#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_network_unixaddr_main(xu_int_t argc, xu_char_t** argv)
{
    // done
    xu_unixaddr_t unixaddr;
    if (xu_unixaddr_cstr_set(&unixaddr, argv[1], xu_false))
    {
        // trace
        xu_trace_i("%s => %{unixaddr}", argv[1], &unixaddr);
    }

    // end
    return 0;
}
