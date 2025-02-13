#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 * macros
 */

// enable lock profiler
#undef XU_LOCK_PROFILER_ENABLE
#if defined(__xu_debug__) && !defined(XU_CONFIG_MICRO_ENABLE)
#    define XU_LOCK_PROFILER_ENABLE
#endif

/* *******************************************************
 * types
 */

// the lock profiler ref type
typedef __xu_typeref__(lock_profiler);

/* *******************************************************
 * interfaces
 */

/*! the lock profiler instance
 *
 * @return              the lock profiler
 */
xu_lock_profiler_ref_t xu_lock_profiler(xu_noarg_t);

/*! init lock profiler
 *
 * @note be used for the debug mode generally
 *
 * @return              the lock profiler
 */
xu_lock_profiler_ref_t xu_lock_profiler_init(xu_noarg_t);

/*! exit lock profiler
 *
 * @param profiler      the lock profiler
 */
xu_void_t xu_lock_profiler_exit(xu_lock_profiler_ref_t profiler);

/*! dump lock profiler
 *
 * @param profiler      the lock profiler
 */
xu_void_t xu_lock_profiler_dump(xu_lock_profiler_ref_t profiler);

/*! register the lock to the lock profiler
 *
 * @param profiler      the lock profiler
 * @param lock          the lock address
 * @param name          the lock name
 */
xu_void_t xu_lock_profiler_register(xu_lock_profiler_ref_t profiler, xu_pointer_t lock, xu_char_t const* name);

/*! the lock be occupied
 *
 * @param profiler      the lock profiler
 * @param lock          the lock address
 */
xu_void_t xu_lock_profiler_occupied(xu_lock_profiler_ref_t profiler, xu_pointer_t lock);
