#pragma once

#include "xutil/macros/type.h"
#include "xutil/prefix.h"

/*! usleep
 *
 * @param us    the microsecond time
 */
xu_void_t xu_usleep(xu_size_t us);

/*! msleep
 *
 * @param ms    the millisecond time
 */
xu_void_t xu_msleep(xu_size_t ms);

/*! sleep
 *
 * @param s     the second time
 */
xu_void_t xu_sleep(xu_size_t s);

/*! clock, ms
 *
 * @return      the mclock
 */
xu_hong_t xu_mclock(xu_noarg_t);

/*! uclock, us
 *
 * @return      the uclock
 */
xu_hong_t xu_uclock(xu_noarg_t);

/*! get the time from 1970-01-01 00:00:00:000
 *
 * @param tv    the timeval
 * @param tz    the timezone
 *
 * @return      xu_true or xu_false
 */
xu_bool_t xu_gettimeofday(xu_timeval_t* tv, xu_timezone_t* tz);
