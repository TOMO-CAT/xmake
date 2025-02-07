#include "xutil/platform/semaphore.h"
#include "xutil/macros/malloc.h"

#if defined(XU_CONFIG_OS_MACOSX) || defined(XU_CONFIG_OS_IOS)

#    include <errno.h>
#    include <mach/mach.h>
#    include <mach/semaphore.h>
#    include <mach/task.h>
#    include <time.h>

/* *******************************************************
 * types
 */
// the semaphore type
typedef struct __xu_semaphore_impl_t
{
    // the semaphore
    semaphore_t semaphore;

    // the value
    xu_atomic32_t value;

} xu_semaphore_impl_t;

/* *******************************************************
 * implementation
 */

xu_semaphore_ref_t xu_semaphore_init(xu_size_t init)
{
    // done
    xu_bool_t            ok   = xu_false;
    xu_semaphore_impl_t* impl = xu_null;
    do
    {
        // make semaphore
        impl = xu_malloc0_type(xu_semaphore_impl_t);
        xu_assert_and_check_break(impl);

        // init semaphore
        if (KERN_SUCCESS != semaphore_create(mach_task_self(), &(impl->semaphore), SYNC_POLICY_FIFO, init)) break;

        // init value
        xu_atomic32_init(&impl->value, (xu_int32_t)init);

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) xu_semaphore_exit((xu_semaphore_ref_t)impl);
        impl = xu_null;
    }

    // ok
    return (xu_semaphore_ref_t)impl;
}
xu_void_t xu_semaphore_exit(xu_semaphore_ref_t semaphore)
{
    xu_semaphore_impl_t* impl = (xu_semaphore_impl_t*)semaphore;
    if (semaphore)
    {
        // exit semaphore
        semaphore_destroy(mach_task_self(), impl->semaphore);

        // exit it
        xu_free(semaphore);
    }
}
xu_bool_t xu_semaphore_post(xu_semaphore_ref_t semaphore, xu_size_t post)
{
    // check
    xu_semaphore_impl_t* impl = (xu_semaphore_impl_t*)semaphore;
    xu_assert_and_check_return_val(semaphore && post, xu_false);

    // post
    while (post--)
    {
        // +2 first
        xu_atomic32_fetch_and_add(&impl->value, 2);

        // signal
        if (KERN_SUCCESS != semaphore_signal(impl->semaphore))
        {
            // restore
            xu_atomic32_fetch_and_sub(&impl->value, 2);
            return xu_false;
        }

        // -1
        xu_atomic32_fetch_and_sub(&impl->value, 1);
    }

    // ok
    return xu_true;
}
xu_long_t xu_semaphore_value(xu_semaphore_ref_t semaphore)
{
    // check
    xu_semaphore_impl_t* impl = (xu_semaphore_impl_t*)semaphore;
    xu_assert_and_check_return_val(semaphore, -1);

    // get value
    return (xu_long_t)xu_atomic32_get(&impl->value);
}
xu_long_t xu_semaphore_wait(xu_semaphore_ref_t semaphore, xu_long_t timeout)
{
    // check
    xu_semaphore_impl_t* impl = (xu_semaphore_impl_t*)semaphore;
    xu_assert_and_check_return_val(semaphore, -1);

    // init timespec
    mach_timespec_t spec = {0};
    if (timeout > 0)
    {
        spec.tv_sec += timeout / 1000;
        spec.tv_nsec += (timeout % 1000) * 1000000;
    }
    else if (timeout < 0)
        spec.tv_sec += 12 * 30 * 24 * 3600; // infinity: one year

    // wait
    xu_long_t ok = semaphore_timedwait(impl->semaphore, spec);

    // timeout or interrupted?
    xu_check_return_val(ok != KERN_OPERATION_TIMED_OUT && ok != KERN_ABORTED, 0);

    // ok?
    xu_check_return_val(ok == KERN_SUCCESS, -1);

    // check value
    xu_assert_and_check_return_val((xu_long_t)xu_atomic32_get(&impl->value) > 0, -1);

    // value--
    xu_atomic32_fetch_and_sub(&impl->value, 1);

    // ok
    return 1;
}

#endif
