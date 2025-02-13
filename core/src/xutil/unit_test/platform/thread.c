#include "xutil/unit_test/ut.h"

/* *******************************************************
 * implementation
 */
static xu_int_t xu_demo_thread_func(xu_cpointer_t priv)
{
    // self
    xu_size_t self = xu_thread_self();

    // trace
    xu_trace_i("thread[%lx: %zu]: init", self, priv);

    // wait some time
    xu_sleep(1);

    // get cpu core
    xu_cpuset_t cpuset;
    XU_CPUSET_ZERO(&cpuset);
    if (xu_thread_getaffinity(xu_null, &cpuset))
    {
        xu_size_t i;
        for (i = 0; i < XU_CPUSET_SIZE; i++)
        {
            if (XU_CPUSET_ISSET(i, &cpuset)) xu_trace_i("thread[%lx: %zu]: get cpu core: %d", self, priv, i);
        }
    }

    // reset thread affinity
    xu_size_t cpu = xu_min(1, xu_cpu_count() - 1);
    XU_CPUSET_ZERO(&cpuset);
    XU_CPUSET_SET(cpu, &cpuset);
    if (!xu_thread_setaffinity(xu_null, &cpuset))
    {
        xu_trace_e("thread[%lx: %zu]: set cpu core(%zu) failed!", self, priv, 1);
    }

    // get cpu core again
    XU_CPUSET_ZERO(&cpuset);
    if (xu_thread_getaffinity(xu_null, &cpuset))
    {
        xu_size_t i;
        for (i = 0; i < XU_CPUSET_SIZE; i++)
        {
            if (XU_CPUSET_ISSET(i, &cpuset)) xu_trace_i("thread[%lx: %zu]: get cpu core again: %d", self, priv, i);
        }
    }

    // exit
    xu_thread_return(-1);

    // trace
    xu_trace_i("thread[%lx: %zu]: exit", self, priv);

    // ok
    return 0;
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_platform_thread_main(xu_int_t argc, xu_char_t** argv)
{
    // get cpu count
    xu_size_t cpu_count = xu_cpu_count();
    xu_trace_i("cpu count: %lu", cpu_count);

    // init threads
    xu_size_t       i = 0;
    xu_cpuset_t     cpuset;
    xu_thread_ref_t threads[64] = {0};
    for (i = 0; i < cpu_count; i++)
    {
        // init thread
        threads[i] = xu_thread_init(xu_null, xu_demo_thread_func, xu_u2p(i), 0);
        xu_assert_and_check_break(threads[i]);

        // set thread affinity
        XU_CPUSET_ZERO(&cpuset);
        XU_CPUSET_SET(i, &cpuset);
        if (!xu_thread_setaffinity(threads[i], &cpuset))
        {
            xu_trace_e("set cpu core(%zu) failed for thread(%zu)", i, i);
        }
    }

    // wait threads
    for (i = 0; i < cpu_count; i++)
    {
        xu_thread_ref_t thread = threads[i];
        if (thread)
        {
            // wait thread
            xu_int_t retval = 0;
            if (xu_thread_wait(thread, -1, &retval) > 0)
            {
                // trace
                xu_trace_i("wait: %zu ok, retval: %d", i, retval);
            }

            // exit thread
            xu_thread_exit(thread);
        }
    }
    return 0;
}
