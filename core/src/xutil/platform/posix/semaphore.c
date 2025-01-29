#include "xutil/platform/semaphore.h"

#include "xutil/macros/assert.h"
#include "xutil/macros/malloc.h"
#include "xutil/prefix.h"

#include <errno.h>
#include <semaphore.h>
#include <time.h>

/* *******************************************************
 * implementation
 */
xu_semaphore_ref_t xu_semaphore_init(xu_size_t init)
{
    // done
    xu_bool_t ok        = xu_false;
    sem_t*    semaphore = xu_null;
    do
    {
        // make semaphore
        semaphore = xu_malloc0_type(sem_t);
        xu_assert_and_check_break(semaphore);

        // init
        if (sem_init(semaphore, 0, init) < 0) break;

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (semaphore) xu_free(semaphore);
        semaphore = xu_null;
    }

    // ok?
    return (xu_semaphore_ref_t)semaphore;
}
xu_void_t xu_semaphore_exit(xu_semaphore_ref_t semaphore)
{
    // check
    sem_t* h = (sem_t*)semaphore;
    xu_assert_and_check_return(h);

    // exit it
    sem_destroy(h);

    // free it
    xu_free(h);
}
xu_bool_t xu_semaphore_post(xu_semaphore_ref_t semaphore, xu_size_t post)
{
    // check
    sem_t* h = (sem_t*)semaphore;
    xu_assert_and_check_return_val(h && post, xu_false);

    // post
    while (post--)
    {
        if (sem_post(h) < 0) return xu_false;
    }

    // ok
    return xu_true;
}
xu_long_t xu_semaphore_value(xu_semaphore_ref_t semaphore)
{
    // check
    sem_t* h = (sem_t*)semaphore;
    xu_assert_and_check_return_val(h, -1);

    // get value
    xu_int_t value = 0;
    return (!sem_getvalue(h, &value)) ? (xu_long_t)value : -1;
}
xu_long_t xu_semaphore_wait(xu_semaphore_ref_t semaphore, xu_long_t timeout)
{
    // check
    sem_t* h = (sem_t*)semaphore;
    xu_assert_and_check_return_val(h, -1);

    // init time
    struct timespec t = {0};
    t.tv_sec          = time(xu_null);
    if (timeout > 0)
    {
        t.tv_sec += timeout / 1000;
        t.tv_nsec += (timeout % 1000) * 1000000;
    }
    else if (timeout < 0)
        t.tv_sec += 12 * 30 * 24 * 3600; // infinity: one year

    // wait semaphore
    xu_long_t r = sem_timedwait(h, &t);

    // ok?
    xu_check_return_val(r, 1);

    // timeout?
    if (errno == EINTR || errno == EAGAIN || errno == ETIMEDOUT) return 0;

    // error
    return -1;
}
