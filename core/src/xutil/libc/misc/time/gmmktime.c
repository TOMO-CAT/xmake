#include "xutil/libc/misc/time/time.h"
#include "xutil/macros/assert.h"

xu_time_t xu_gmmktime(xu_tm_t const* tm)
{
    // check
    xu_assert_and_check_return_val(tm, -1);

    // done
    xu_long_t y = tm->year;
    xu_long_t m = tm->month;
    xu_long_t d = tm->mday;

    if (m < 3)
    {
        m += 12;
        y--;
    }

    xu_time_t time = 86400 * (d + (153 * m - 457) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 719469);
    time += 3600 * tm->hour;
    time += 60 * tm->minute;
    time += tm->second;

    // time
    return time;
}
