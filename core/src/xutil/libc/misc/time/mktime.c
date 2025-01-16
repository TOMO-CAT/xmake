#include "xutil/libc/misc/time/time.h"
#include "xutil/macros/assert.h"
#ifdef XU_CONFIG_LIBC_HAVE_MKTIME
#    include <time.h>
#endif

xu_time_t xu_mktime(xu_tm_t const* tm)
{
    // check
    xu_assert_and_check_return_val(tm, -1);

#ifdef XU_CONFIG_LIBC_HAVE_MKTIME
    // init
    struct tm t = {0};
    t.tm_sec    = (xu_int_t)tm->second;
    t.tm_min    = (xu_int_t)tm->minute;
    t.tm_hour   = (xu_int_t)tm->hour;
    t.tm_mday   = (xu_int_t)tm->mday;
    t.tm_mon    = (xu_int_t)tm->month - 1;
    t.tm_year   = (xu_int_t)(tm->year > 1900 ? tm->year - 1900 : tm->year);
    t.tm_wday   = (xu_int_t)tm->week;
    t.tm_yday   = (xu_int_t)tm->yday;
    t.tm_isdst  = (xu_int_t)tm->isdst;

    // mktime
    return (xu_time_t)mktime(&t);
#else
    // GMT+8 for beijing.china.
    xu_time_t time = xu_gmmktime(tm);
    return time >= 8 * 3600 ? time - 8 * 3600 : -1;
#endif
}
