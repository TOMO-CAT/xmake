#include "xutil/libc/misc/time/time.h"

#ifdef XU_CONFIG_LIBC_HAVE_LOCALTIME
#    include <time.h>
#endif

xu_bool_t xu_localtime(xu_time_t time, xu_tm_t* tm)
{
#ifdef XU_CONFIG_LIBC_HAVE_LOCALTIME
    // localtime
    time_t     t   = (time_t)time;
    struct tm* ptm = localtime(&t);
    if (ptm && tm)
    {
        tm->second = ptm->tm_sec;
        tm->minute = ptm->tm_min;
        tm->hour   = ptm->tm_hour;
        tm->mday   = ptm->tm_mday;
        tm->month  = ptm->tm_mon + 1;
        tm->year   = ptm->tm_year + 1900;
        tm->week   = ptm->tm_wday;
        tm->yday   = ptm->tm_yday;
        tm->isdst  = ptm->tm_isdst;
    }

    // ok?
    return ptm ? xu_true : xu_false;
#else
    // GMT+8 for beijing.china.
    time += 8 * 3600;
    return xu_gmtime(time, tm);
#endif
}
