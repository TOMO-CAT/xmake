#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_libc_time_main(xu_int_t argc, xu_char_t** argv)
{
    // time
    xu_time_t now = xu_time();

    // the local time
    xu_tm_t lt = {0};
    if (xu_localtime(now, &lt))
    {
        xu_trace_i("LMT: %04ld-%02ld-%02ld %02ld:%02ld:%02ld, week: %d, time: %lld ?= %lld", lt.year, lt.month, lt.mday,
                   lt.hour, lt.minute, lt.second, lt.week, xu_mktime(&lt), now);
    }

    // the gmt time
    xu_tm_t gt = {0};
    if (xu_gmtime(now, &gt))
    {
        xu_trace_i("GMT: %04ld-%02ld-%02ld %02ld:%02ld:%02ld, week: %d, time: %lld ?= %lld", gt.year, gt.month, gt.mday,
                   gt.hour, gt.minute, gt.second, gt.week, xu_gmmktime(&gt), now);
    }

    return 0;
}
