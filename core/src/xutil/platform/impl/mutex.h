#pragma once

#include "xutil/config.h"

#if defined(XU_CONFIG_POSIX_HAVE_PTHREAD_MUTEX_INIT)
#    include <pthread.h>
#else
#    include "xutil/platform/spinlock.h"
#endif

/* *******************************************************
 *  types
 * *******************************************************
 */
#if defined(XU_CONFIG_POSIX_HAVE_PTHREAD_MUTEX_INIT)
typedef pthread_mutex_t xu_mutex_t;
#else
typedef xu_spinlock_t xu_mutex_t;
#endif

/* *******************************************************
 *  interfaces
 * *******************************************************
 */
/* init mutex at the given mutex data
 *
 * @param       the mutex data
 *
 * @return      the mutex reference
 */
xu_mutex_ref_t xu_mutex_init_impl(xu_mutex_t* mutex);

/* exit mutex
 *
 * @param       the mutex data
 */
xu_void_t xu_mutex_exit_impl(xu_mutex_t* mutex);

/* enter mutex without profiler
 *
 * @param mutex the mutex
 *
 * @return      xu_true or xu_false
 */
xu_bool_t xu_mutex_enter_without_profiler(xu_mutex_ref_t mutex);

/* try to enter mutex without profiler
 *
 * @param mutex the mutex
 *
 * @return      xu_true or xu_false
 */
xu_bool_t xu_mutex_entry_try_without_profiler(xu_mutex_ref_t mutex);
