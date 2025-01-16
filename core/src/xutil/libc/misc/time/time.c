#include "xutil/libc/misc/time/time.h"
#include "xutil/platform/time.h"

xu_time_t xu_time()
{
    // time
    xu_timeval_t tv = {0};
    return xu_gettimeofday(&tv, xu_null) ? tv.tv_sec : (xu_time_t)-1;
}
