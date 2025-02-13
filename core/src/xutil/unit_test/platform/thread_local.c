#include "xutil/unit_test/ut.h"

/* *******************************************************
 * implementation
 */
static xu_void_t xu_demo_thread_local_free(xu_cpointer_t priv)
{
    xu_trace_i("thread[%lx]: free: %p", xu_thread_self(), priv);
}
static xu_int_t xu_demo_thread_local_test(xu_cpointer_t priv)
{
    // self
    xu_size_t self = xu_thread_self();

    // trace
    xu_trace_i("thread[%lx]: init", self);

    // init the thread local, only once
    static xu_thread_local_t s_local = XU_THREAD_LOCAL_INIT;
    if (!xu_thread_local_init(&s_local, xu_demo_thread_local_free)) return -1;

    // init start time
    xu_hong_t time = xu_mclock();

    // done
    __xu_volatile__ xu_size_t count = 10000000;
    while (count--)
    {
        // attempt to get local variable
        xu_size_t local;
        if (!(local = (xu_size_t)xu_thread_local_get(&s_local)))
        {
            // init local variable
            if (xu_thread_local_set(&s_local, (xu_cpointer_t)self)) local = self;
        }

        // check
        if (local != self)
        {
            // trace
            xu_trace_i("thread[%lx]: invalid value: %lx", self, local);
        }
    }

    // compile the interval time
    time = xu_mclock() - time;

    // trace
    xu_trace_i("thread[%lx]: exit: %lld ms", self, time);

    // ok
    return 0;
}
static xu_int_t xu_demo_thread_local_stub(xu_cpointer_t priv)
{
    // self
    xu_size_t self = xu_thread_self();

    // trace
    xu_trace_i("thread[%lx]: init", self);

    // trace
    xu_trace_i("thread[%lx]: exit", self);

    // ok
    return 0;
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_platform_thread_local_main(xu_int_t argc, xu_char_t** argv)
{
    xu_thread_init(xu_null, xu_demo_thread_local_test, xu_null, 0);
    xu_thread_init(xu_null, xu_demo_thread_local_test, xu_null, 0);
    xu_thread_init(xu_null, xu_demo_thread_local_test, xu_null, 0);
    xu_thread_init(xu_null, xu_demo_thread_local_test, xu_null, 0);
    xu_thread_init(xu_null, xu_demo_thread_local_stub, xu_null, 0);

    // wait
    xu_getchar();
    return 0;
}
