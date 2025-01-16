#pragma once

#include "xutil/libc/misc/time/type.h"
#include "xutil/prefix.h"

/*! the time as the number of seconds since the epoch, 1970-01-01 00:00:00 +0000 (utc)
 *
 * @return              the returned time or -1
 */
xu_time_t xu_time(xu_noarg_t);

/*! the gmt time
 *
 * @param time          the time value
 * @param tm            the gmt time pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_gmtime(xu_time_t time, xu_tm_t* tm);

/*! the local time
 *
 * @param time          the time value
 * @param tm            the local time pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_localtime(xu_time_t time, xu_tm_t* tm);

/*! make the time value from the local time
 *
 * @param tm            the time
 *
 * @return              the time value
 */
xu_time_t xu_mktime(xu_tm_t const* tm);

/*! make the time value from the gmt time
 *
 * @param tm            the time
 *
 * @return              the time value
 */
xu_time_t xu_gmmktime(xu_tm_t const* tm);
