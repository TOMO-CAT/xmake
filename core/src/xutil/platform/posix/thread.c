#include "xutil/platform/thread.h"
#include "xutil/macros/malloc.h"
#include "xutil/utils/trace.h"
#include "xutil/utils/utils.h"

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>

/* *******************************************************
 * types
 * *******************************************************
 */

// the thread type
typedef struct __xu_thread_t
{
    // the pthread, @note We must put it at the beginning, because posix/mach/thread_affinity will use it.
    pthread_t pthread;

    // is joined?
    xu_bool_t is_joined;

} xu_thread_t;

/* *******************************************************
 * declaration
 * *******************************************************
 */

xu_void_t xu_thread_local_clear_atexit();

/* *******************************************************
 * implementation
 * *******************************************************
 */

xu_void_t xu_thread_exit(xu_thread_ref_t self)
{
    // check
    xu_thread_t* thread = (xu_thread_t*)self;
    xu_assert_and_check_return(thread);

    // detach thread if not joined
    if (!thread->is_joined) pthread_detach(thread->pthread);
    xu_free(thread);
}
xu_long_t xu_thread_wait(xu_thread_ref_t self, xu_long_t timeout, xu_int_t* retval)
{
    // check
    xu_thread_t* thread = (xu_thread_t*)self;
    xu_assert_and_check_return_val(thread, -1);

    // wait thread
    xu_long_t    ok  = -1;
    xu_pointer_t ret = xu_null;
    if ((ok = pthread_join(thread->pthread, &ret)) && ok != ESRCH)
    {
        // trace
        xu_trace_e("thread[%p]: wait failed: %ld, errno: %d", thread, ok, errno);

        // join failed? detach it
        pthread_detach(thread->pthread);
        return -1;
    }

    // join ok
    thread->is_joined = xu_true;

    // save the return value
    if (retval) *retval = xu_p2s32(ret);

    // ok
    return 1;
}
xu_void_t xu_thread_return(xu_int_t value)
{
    // free all thread local data on the current thread
    xu_thread_local_clear_atexit();

    // exit thread and return value
    pthread_exit((xu_pointer_t)(xu_long_t)value);
}
xu_bool_t xu_thread_suspend(xu_thread_ref_t self)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_thread_resume(xu_thread_ref_t self)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_size_t xu_thread_self() { return (xu_size_t)pthread_self(); }
