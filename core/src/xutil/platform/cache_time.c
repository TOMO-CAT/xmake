#include "xutil/platform/cache_time.h"
#include "xutil/platform/atomic.h"
#include "xutil/platform/time.h"

/* *******************************************************
 * globals
 * *******************************************************
 */

// the cached time
static xu_atomic64_t g_time = 0;

/* *******************************************************
 * implementation
 * *******************************************************
 */
xu_hong_t xu_cache_time_spak()
{
    // get the time
    xu_timeval_t tv = {0};
    if (!xu_gettimeofday(&tv, xu_null)) return -1;

    // the time value
    xu_hong_t val = ((xu_hong_t)tv.tv_sec * 1000 + tv.tv_usec / 1000);

    // save it
    xu_atomic64_set(&g_time, val);

    // ok
    return val;
}
xu_hong_t xu_cache_time_mclock()
{
    xu_hong_t t;
    if (!(t = (xu_hong_t)xu_atomic64_get(&g_time))) t = xu_cache_time_spak();
    return t;
}
xu_hong_t xu_cache_time_sclock() { return xu_cache_time_mclock() / 1000; }
xu_time_t xu_cache_time() { return (xu_time_t)xu_cache_time_sclock(); }
