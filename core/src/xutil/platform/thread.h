#pragma once

#include "xutil/macros/type.h"
#include "xutil/platform/sched.h"
#include "xutil/prefix.h"

/* *******************************************************
 *  types
 * *******************************************************
 */
/*! the thread func type
 *
 * @param priv          the passed private data
 *
 * @return              the return value
 */
typedef xu_int_t (*xu_thread_func_t)(xu_cpointer_t priv);

/* *******************************************************
 *  interfaces
 * *******************************************************
 */

/*! init thread
 *
 * @code
    static xu_int_t xu_demo_thread_func(xu_cpointer_t priv)
    {
        // self
        xu_size_t self = xu_thread_self();

        // trace
        xu_trace_i("thread[%lx: %s]: init", priv, self);

        // exit thread and return failed value: -1
        // xu_thread_return(-1);

        // trace
        xu_trace_i("thread[%lx: %s]: exit", priv, self);

        // ok
        return 0;
    }

    // init thread
    xu_thread_ref_t thread = xu_thread_init(xu_null, xu_demo_thread_func, "hello", 0);
    if (thread)
    {
        // wait thread
        xu_int_t retval = 0;
        if (xu_thread_wait(thread, -1, &retval) > 0)
        {
            // trace
            xu_trace_i("wait: ok, retval: %d", retval);
        }

        // exit thread
        xu_thread_exit(thread);
    }
 * @endcode
 *
 * @param name          the thread name, maybe null
 * @param func          the thread func
 * @param priv          the thread func private data
 * @param stack         the thread stack, using the default stack size if be zero
 *
 * @return              the thread handle
 */
xu_thread_ref_t xu_thread_init(xu_char_t const* name, xu_thread_func_t func, xu_cpointer_t priv, xu_size_t stack);

/*! exit thread
 *
 * @param thread        the thread
 */
xu_void_t xu_thread_exit(xu_thread_ref_t thread);

/*! wait thread
 *
 * @param thread        the thread
 * @param timeout       the timeout
 * @param retval        the return value pointer of the thread (optional)
 *
 * @return              ok: 1, timeout: 0, error: -1
 */
xu_long_t xu_thread_wait(xu_thread_ref_t thread, xu_long_t timeout, xu_int_t* retval);

/*! suspend thread
 *
 * @param thread        the thread
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_thread_suspend(xu_thread_ref_t thread);

/*! resume thread
 *
 * @param thread        the thread
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_thread_resume(xu_thread_ref_t thread);

/*! the self thread identifier
 *
 * @return              the self thread identifier
 */
xu_size_t xu_thread_self(xu_noarg_t);

/*! return the thread value
 *
 * @param value         the return value of the thread
 */
xu_void_t xu_thread_return(xu_int_t value);

/*! run the given function only once
 *
 * @code

    // the once function
    static xu_bool_t xu_thread_once_func(xu_cpointer_t priv)
    {
        // trace
        xu_trace_i("%s", priv);

        // ok
        return xu_true;
    }

    // run the once function
    static xu_atomic32_t once = 0;
    if (xu_thread_once(&once, xu_thread_once_func, "hello"))
    {
        // ok
        // ...
    }

 * @endcode
 *
 * @param lock          the global or static atomic lock pointer (need be initialized as zero)
 * @param func          the function
 * @param priv          the user private data
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_thread_once(xu_atomic32_t* lock, xu_bool_t (*func)(xu_cpointer_t), xu_cpointer_t priv);

/*! set cpu affinity for the given thread
 *
 * @param thread        the thread, set the current thread if be null
 * @param cpuset        the cpu set
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_thread_setaffinity(xu_thread_ref_t thread, xu_cpuset_ref_t cpuset);

/*! get cpu affinity from the given thread
 *
 * @param thread        the thread, set the current thread if be null
 * @param cpuset        the cpu set
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_thread_getaffinity(xu_thread_ref_t thread, xu_cpuset_ref_t cpuset);
