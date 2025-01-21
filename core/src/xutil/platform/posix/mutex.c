#include "xutil/platform/mutex.h"
#include "xutil/macros/assert.h"
#include "xutil/macros/malloc.h"
#include "xutil/platform/impl/mutex.h"

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>

/* *******************************************************
 *  private implementation
 * *******************************************************
 */

xu_mutex_ref_t xu_mutex_init_impl(xu_mutex_t* mutex)
{
    // init mutex, @note we cannot use asset/trace because them will use mutex
    xu_assert_static(sizeof(pthread_mutex_t) == sizeof(xu_mutex_t));
    return (mutex && !pthread_mutex_init(mutex, xu_null)) ? ((xu_mutex_ref_t)mutex) : xu_null;
}
xu_void_t xu_mutex_exit_impl(xu_mutex_t* mutex)
{
    // exit it
    if (mutex) pthread_mutex_destroy(mutex);
}
xu_bool_t xu_mutex_enter_without_profiler(xu_mutex_ref_t mutex)
{
    // check, @note we cannot use asset/trace because them will use mutex
    xu_check_return_val(mutex, xu_false);

    // enter
    return pthread_mutex_lock((pthread_mutex_t*)mutex) == 0;
}
xu_bool_t xu_mutex_entry_try_without_profiler(xu_mutex_ref_t mutex)
{
    // check, @note we cannot use asset/trace because them will use mutex
    xu_check_return_val(mutex, xu_false);

    // try to enter
    return pthread_mutex_trylock((pthread_mutex_t*)mutex) == 0;
}

/* *******************************************************
 *  implementation
 * *******************************************************
 */
xu_mutex_ref_t xu_mutex_init()
{
    // make mutex
    xu_mutex_t* pmutex = xu_malloc0(sizeof(xu_mutex_t));
    xu_assert_and_check_return_val(pmutex, xu_null);

    // init mutex
    xu_mutex_ref_t mutex = xu_mutex_init_impl(pmutex);
    if (mutex)
        return mutex;
    else
    {
        if (pmutex) xu_free((xu_pointer_t)pmutex);
        return xu_null;
    }
}
xu_void_t xu_mutex_exit(xu_mutex_ref_t mutex)
{
    // check
    xu_assert_and_check_return(mutex);

    // exit it
    xu_mutex_t* pmutex = (xu_mutex_t*)mutex;
    if (pmutex)
    {
        xu_mutex_exit_impl(pmutex);
        xu_free((xu_pointer_t)pmutex);
    }
}
xu_bool_t xu_mutex_enter(xu_mutex_ref_t mutex)
{
    // try to enter for profiler
#ifdef XU_LOCK_PROFILER_ENABLE
    if (xu_mutex_enter_try(mutex)) return xu_true;
#endif

    // enter
    return xu_mutex_enter_without_profiler(mutex);
}
xu_bool_t xu_mutex_enter_try(xu_mutex_ref_t mutex)
{
    // try to enter
    if (!xu_mutex_entry_try_without_profiler(mutex))
    {
        // occupied
#ifdef XU_LOCK_PROFILER_ENABLE
        xu_lock_profiler_occupied(xu_lock_profiler(), (xu_handle_t)mutex);
#endif
        return xu_false;
    }
    return xu_true;
}
xu_bool_t xu_mutex_leave(xu_mutex_ref_t mutex)
{
    // check, @note we cannot use asset/trace because them will use mutex
    xu_check_return_val(mutex, xu_false);

    // leave
    return pthread_mutex_unlock((pthread_mutex_t*)mutex) == 0;
}
