#include "xutil/platform/time.h"

#if defined(XU_CONFIG_LIBC_HAVE_GETTIMEOFDAY)
#    include "xutil/platform/posix/time.c"
#else
xu_void_t xu_usleep(xu_size_t us) { xu_trace_noimpl(); }
xu_void_t xu_msleep(xu_size_t ms) { xu_trace_noimpl(); }
xu_void_t xu_sleep(xu_size_t s) { xu_trace_noimpl(); }
xu_hong_t xu_mclock()
{
    xu_trace_noimpl();
    return 0;
}
xu_hong_t xu_uclock()
{
    xu_trace_noimpl();
    return 0;
}
xu_bool_t xu_gettimeofday(xu_timeval_t* tv, xu_timezone_t* tz)
{
    xu_trace_noimpl();
    return xu_false;
}
#endif
