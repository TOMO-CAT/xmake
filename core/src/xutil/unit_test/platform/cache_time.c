#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_platform_cache_time_main(xu_int_t argc, xu_char_t** argv)
{
    xu_trace_i("%lld %lld", xu_cache_time_spak(), xu_cache_time_mclock());
    xu_sleep(1);
    xu_trace_i("%lld %lld", xu_cache_time_spak(), xu_cache_time_mclock());
    xu_sleep(1);
    xu_trace_i("%lld %lld", xu_cache_time_spak(), xu_cache_time_mclock());
    return 0;
}
