#include "xutil/platform/mutex.h"

#if defined(XU_CONFIG_POSIX_HAVE_PTHREAD_MUTEX_INIT)
#    include "xutil/platform/posix/mutex.c"
#else
#    include "impl/mutex.h"
xu_mutex_ref_t xu_mutex_init_impl(xu_mutex_t* mutex)
{
    // init mutex, @note we cannot use asset/trace because them will use mutex
    xu_assert_static(sizeof(xu_spinlock_t) == sizeof(xu_mutex_t));
    return (mutex && xu_spinlock_init(mutex)) ? ((xu_mutex_ref_t)mutex) : xu_null;
}
xu_void_t xu_mutex_exit_impl(xu_mutex_t* mutex)
{
    // exit it
    if (mutex) xu_spinlock_exit(mutex);
}
xu_bool_t xu_mutex_enter_without_profiler(xu_mutex_ref_t mutex)
{
    // check, @note we cannot use asset/trace because them will use mutex
    xu_check_return_val(mutex, xu_false);

    // enter
    xu_spinlock_enter_without_profiler((xu_spinlock_ref_t)mutex);
    return xu_true;
}
xu_bool_t xu_mutex_entry_try_without_profiler(xu_mutex_ref_t mutex)
{
    // check, @note we cannot use asset/trace because them will use mutex
    xu_check_return_val(mutex, xu_false);

    // try to enter
    return xu_spinlock_enter_try_without_profiler((xu_spinlock_ref_t)mutex);
}
xu_mutex_ref_t xu_mutex_init()
{
    // done
    xu_bool_t         ok   = xu_false;
    xu_spinlock_ref_t lock = xu_null;
    do
    {
        // make lock
        lock = xu_malloc0_type(xu_spinlock_t);
        xu_assert_and_check_break(lock);

        // init lock
        if (!xu_spinlock_init(lock)) break;

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        xu_free(lock);
        lock = xu_null;
    }

    // ok?
    return (xu_mutex_ref_t)lock;
}
xu_void_t xu_mutex_exit(xu_mutex_ref_t mutex)
{
    // check
    xu_assert_and_check_return(mutex);

    // exit it
    xu_spinlock_ref_t lock = (xu_spinlock_ref_t)mutex;
    if (lock)
    {
        // exit lock
        xu_spinlock_exit(lock);

        // free it
        xu_free(lock);
    }
}
xu_bool_t xu_mutex_enter(xu_mutex_ref_t mutex)
{
    // check, @note we cannot use asset/trace because them will use mutex
    xu_check_return_val(mutex, xu_false);

    // enter
    xu_spinlock_enter((xu_spinlock_ref_t)mutex);
    return xu_true;
}
xu_bool_t xu_mutex_enter_try(xu_mutex_ref_t mutex)
{
    // check, @note we cannot use asset/trace because them will use mutex
    xu_check_return_val(mutex, xu_false);

    // try to enter
    return xu_spinlock_enter_try((xu_spinlock_ref_t)mutex);
}
xu_bool_t xu_mutex_leave(xu_mutex_ref_t mutex)
{
    // check, @note we cannot use asset/trace because them will use mutex
    xu_check_return_val(mutex, xu_false);

    // leave
    xu_spinlock_leave((xu_spinlock_ref_t)mutex);
    return xu_true;
}
#endif
