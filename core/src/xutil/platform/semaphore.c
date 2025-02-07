#include "xutil/platform/semaphore.h"

#include "xutil/platform/atomic.h"
#include "xutil/platform/cache_time.h"
#include "xutil/platform/time.h"

/* *******************************************************
 * implementation
 */

#if defined(XU_CONFIG_OS_MACOSX) || defined(XU_CONFIG_OS_IOS)
#    include "xutil/platform/mach/semaphore.c"
#elif defined(XU_CONFIG_POSIX_HAVE_SEM_INIT)
#    include "xutil/platform/posix/semaphore.c"
#elif defined(XU_CONFIG_SYSTEMV_HAVE_SEMGET) && defined(XU_CONFIG_SYSTEMV_HAVE_SEMTIMEDOP)
#    include "xutil/platform/systemv/semaphore.c"
#else
xu_semaphore_ref_t xu_semaphore_init(xu_size_t value)
{
    // make
    xu_atomic32_t* semaphore = xu_malloc0_type(xu_atomic32_t);
    xu_assert_and_check_return_val(semaphore, xu_null);

    // init
    xu_atomic32_init(semaphore, (xu_int32_t)value);

    // ok
    return (xu_semaphore_ref_t)semaphore;
}
xu_void_t xu_semaphore_exit(xu_semaphore_ref_t self)
{
    // check
    xu_atomic32_t* semaphore = (xu_atomic32_t*)self;
    xu_assert_and_check_return(semaphore);

    // free it
    xu_free(semaphore);
}
xu_bool_t xu_semaphore_post(xu_semaphore_ref_t self, xu_size_t post)
{
    // check
    xu_atomic32_t* semaphore = (xu_atomic32_t*)self;
    xu_assert_and_check_return_val(semaphore && post, xu_false);

    // post it
    xu_int32_t value = xu_atomic32_fetch_and_add(semaphore, (xu_int32_t)post);

    // ok
    return value >= 0 ? xu_true : xu_false;
}
xu_long_t xu_semaphore_value(xu_semaphore_ref_t self)
{
    // check
    xu_atomic32_t* semaphore = (xu_atomic32_t*)self;
    xu_assert_and_check_return_val(semaphore, xu_false);

    // get value
    return (xu_long_t)xu_atomic32_get(semaphore);
}
xu_long_t xu_semaphore_wait(xu_semaphore_ref_t self, xu_long_t timeout)
{
    // check
    xu_atomic32_t* semaphore = (xu_atomic32_t*)self;
    xu_assert_and_check_return_val(semaphore, -1);

    // init
    xu_long_t r    = 0;
    xu_hong_t base = xu_cache_time_spak();

    // wait
    while (1)
    {
        // get post
        xu_long_t post = (xu_long_t)xu_atomic32_get(semaphore);

        // has signal?
        if (post > 0)
        {
            // semaphore--
            xu_atomic32_fetch_and_sub(semaphore, 1);

            // ok
            r = post;
            break;
        }
        // no signal?
        else if (!post)
        {
            // timeout?
            if (timeout >= 0 && xu_cache_time_spak() - base >= timeout)
                break;
            else
                xu_msleep(200);
        }
        // error
        else
        {
            r = -1;
            break;
        }
    }

    return r;
}
#endif
