#include "xutil/platform/thread.h"
#include "xutil/macros/malloc.h"
#include "xutil/platform/atomic32.h"
#include "xutil/platform/impl/thread_local.h"
#include "xutil/platform/thread_local.h"
#include "xutil/platform/time.h"
#include "xutil/utils/trace.h"

// the return value type of the thread function
typedef xu_pointer_t xu_thread_retval_t;

xu_void_t xu_thread_local_clear_atexit();

#ifndef XU_CONFIG_MICRO_ENABLE
static xu_bool_t xu_thread_local_free(xu_iterator_ref_t iterator, xu_pointer_t item, xu_cpointer_t priv)
{
    // the local
    xu_thread_local_ref_t local = (xu_thread_local_ref_t)item;
    if (local)
    {
        // free the thread local data
        if (local->free && xu_thread_local_has(local))
        {
            // free it
            local->free(xu_thread_local_get(local));
        }
    }

    // ok
    return xu_true;
}
#endif

xu_void_t xu_thread_local_clear_atexit()
{
#ifndef XU_CONFIG_MICRO_ENABLE
    // free all thread local data on the current thread
    xu_thread_local_walk(xu_thread_local_free, xu_null);
#endif
}

static xu_thread_retval_t xu_thread_func(xu_pointer_t priv)
{
    // done
    xu_thread_retval_t retval = (xu_thread_retval_t)0;
    xu_value_ref_t     args   = (xu_value_ref_t)priv;
    do
    {
        // check
        xu_assert_and_check_break(args);

        // get the thread function
        xu_thread_func_t func = (xu_thread_func_t)args[0].ptr;
        xu_assert_and_check_break(func);

        // get the thread private data
        xu_cpointer_t priv = args[1].ptr;

        // free arguments before calling thread function
        if (args) xu_free(args);
        args = xu_null;

        // call the thread function
        retval = (xu_thread_retval_t)(xu_size_t)func(priv);

        // free all thread local data on the current thread
        xu_thread_local_clear_atexit();

    } while (0);

    // return the return value
    return retval;
}

#if defined(XU_CONFIG_POSIX_HAVE_PTHREAD_CREATE)
#    include "xutil/platform/posix/thread.c"
xu_thread_ref_t xu_thread_init(xu_char_t const* name, xu_thread_func_t func, xu_cpointer_t priv, xu_size_t stack)
{
    // check
    xu_assert_and_check_return_val(func, xu_null);

    pthread_attr_t attr;
    xu_bool_t      ok     = xu_false;
    xu_value_ref_t args   = xu_null;
    xu_thread_t*   thread = xu_null;
    do
    {
        // init thread
        thread = xu_malloc0_type(xu_thread_t);
        xu_assert_and_check_break(thread);

        // init attr
        if (stack)
        {
            if (pthread_attr_init(&attr)) break;
            pthread_attr_setstacksize(&attr, stack);
        }

        // init arguments
        args = xu_nalloc0_type(2, xu_value_t);
        xu_assert_and_check_break(args);

        // save function and private data
        args[0].ptr = (xu_pointer_t)func;
        args[1].ptr = (xu_pointer_t)priv;

        // init thread
        if (pthread_create(&thread->pthread, stack ? &attr : xu_null, xu_thread_func, args)) break;

        // ok
        ok = xu_true;

    } while (0);

    // exit attr
    if (stack) pthread_attr_destroy(&attr);

    // failed
    if (!ok)
    {
        // exit arguments
        if (args) xu_free(args);
        args = xu_null;

        // exit thread
        if (thread) xu_free(thread);
        thread = xu_null;
    }

    // ok?
    return ok ? ((xu_thread_ref_t)thread) : xu_null;
}

#else
xu_thread_ref_t xu_thread_init(xu_char_t const* name, xu_thread_func_t func, xu_cpointer_t priv, xu_size_t stack)
{
    xu_used(xu_thread_func);
    xu_trace_noimpl();
    return xu_null;
}
xu_void_t xu_thread_exit(xu_thread_ref_t thread) { xu_trace_noimpl(); }
xu_long_t xu_thread_wait(xu_thread_ref_t thread, xu_long_t timeout, xu_int_t* retval)
{
    xu_trace_noimpl();
    return -1;
}
xu_void_t xu_thread_return(xu_int_t value) { xu_trace_noimpl(); }
xu_bool_t xu_thread_suspend(xu_thread_ref_t thread)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_thread_resume(xu_thread_ref_t thread)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_size_t xu_thread_self()
{
    xu_trace_noimpl();
    return 0;
}
#endif

#if defined(XU_CONFIG_OS_MACOSX) || defined(XU_CONFIG_OS_IOS)
#    include "xutil/platform/mach/thread_affinity.c"
#elif defined(XU_CONFIG_POSIX_HAVE_PTHREAD_SETAFFINITY_NP)
#    include "xutil/platform/posix/thread_affinity.c"
#else
xu_bool_t xu_thread_setaffinity(xu_thread_ref_t thread, xu_cpuset_ref_t cpuset)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_thread_getaffinity(xu_thread_ref_t thread, xu_cpuset_ref_t cpuset)
{
    xu_trace_noimpl();
    return xu_false;
}
#endif

xu_bool_t xu_thread_once(xu_atomic32_t* lock, xu_bool_t (*func)(xu_cpointer_t), xu_cpointer_t priv)
{
    // check
    xu_check_return_val(lock && func, xu_false);

    /* called?
     *
     * 0: have been not called
     * 1: be calling
     * 2: have been called and ok
     * -2: have been called and failed
     */
    xu_atomic32_t called = xu_atomic32_fetch_and_cmpset(lock, 0, 1);

    // called?
    if (called && called != 1) return called == 2;
    // have been not called? call it
    else if (!called)
    {
        // call the once function
        xu_bool_t ok = func(priv);

        // call ok
        xu_atomic32_set(lock, ok ? 2 : -1);

        // ok?
        return ok;
    }
    // calling? wait it
    else
    {
        // try getting it
        xu_size_t tryn = 50;
        while ((1 == xu_atomic32_get(lock)) && tryn--)
        {
            // wait some time
            xu_msleep(100);
        }
    }

    // ok? 1: timeout, -2: failed, 2: ok
    return xu_atomic32_get(lock) == 2;
}
