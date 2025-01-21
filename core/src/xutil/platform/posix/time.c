#include "xutil/platform/time.h"
#include "xutil/macros/type.h"
#include "xutil/prefix.h"
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

xu_void_t xu_usleep(xu_size_t us) { usleep(us); }

xu_void_t xu_msleep(xu_size_t ms)
{
#if defined(XU_CONFIG_MODULE_HAVE_COROUTINE) && !defined(XU_CONFIG_MICRO_ENABLE)
    // attempt to sleep in coroutine
    if (xu_coroutine_self())
    {
        // sleep it
        xu_coroutine_sleep(ms);
        return;
    }
#endif

    // sleep it
    xu_usleep(ms * 1000);
}

xu_void_t xu_sleep(xu_size_t s) { xu_msleep(s * 1000); }
xu_hong_t xu_mclock()
{
    xu_timeval_t tv = {0};
    if (!xu_gettimeofday(&tv, xu_null)) return -1;
    return ((xu_hong_t)tv.tv_sec * 1000 + tv.tv_usec / 1000);
}
xu_hong_t xu_uclock()
{
    xu_timeval_t tv = {0};
    if (!xu_gettimeofday(&tv, xu_null)) return -1;
    return ((xu_hong_t)tv.tv_sec * 1000000 + tv.tv_usec);
}
xu_bool_t xu_gettimeofday(xu_timeval_t* tv, xu_timezone_t* tz)
{
    // gettimeofday
    struct timeval  ttv = {0};
    struct timezone ttz = {0};
    if (gettimeofday(&ttv, &ttz)) return xu_false;

    // tv
    if (tv)
    {
        tv->tv_sec  = (xu_time_t)ttv.tv_sec;
        tv->tv_usec = (xu_suseconds_t)ttv.tv_usec;
    }

    // tz
    if (tz)
    {
        tz->tz_minuteswest = ttz.tz_minuteswest;
        tz->tz_dsttime     = ttz.tz_dsttime;
    }

    // ok
    return xu_true;
}
