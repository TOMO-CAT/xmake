#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 * interfaces
 * *******************************************************
 */

/*! the cached time, like xu_time
 *
 * lower accuracy and faster
 *
 * @return          the now time, s
 */
xu_time_t xu_cache_time(xu_noarg_t);

/*! spak cached time
 *
 * update the cached time for the external loop thread
 *
 * @return          the now ms-clock
 */
xu_hong_t xu_cache_time_spak(xu_noarg_t);

/*! the cached ms-clock
 *
 * lower accuracy and faster
 *
 * @return          the now ms-clock
 */
xu_hong_t xu_cache_time_mclock(xu_noarg_t);

/*! the cached s-clock
 *
 * lower accuracy and faster
 *
 * @return          the now s-clock
 */
xu_hong_t xu_cache_time_clock(xu_noarg_t);
