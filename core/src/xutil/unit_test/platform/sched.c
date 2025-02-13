#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_platform_sched_main(xu_int_t argc, xu_char_t** argv)
{
    // get cpu count
    xu_size_t cpu_count = xu_cpu_count();
    xu_trace_i("cpu count: %lu", cpu_count);

    // get the previous cpu core
    xu_cpuset_t cpuset;
    XU_CPUSET_ZERO(&cpuset);
    if (xu_sched_getaffinity(0, &cpuset))
    {
        xu_size_t i;
        for (i = 0; i < cpu_count; i++)
        {
            if (XU_CPUSET_ISSET(i, &cpuset)) xu_trace_i("get previous cpu core: %d", i);
        }
    }

    // trace
    xu_int_t cpu = argv[1] ? xu_atoi(argv[1]) : 0;
    xu_trace_i("set cpu core: %d", cpu);

    // set cpu affinity to the given cpu core
    XU_CPUSET_ZERO(&cpuset);
    XU_CPUSET_SET(cpu, &cpuset);
    if (xu_sched_setaffinity(0, &cpuset))
    {
        XU_CPUSET_ZERO(&cpuset);
        if (xu_sched_getaffinity(0, &cpuset))
        {
            xu_size_t i;
            for (i = 0; i < cpu_count; i++)
            {
                if (XU_CPUSET_ISSET(i, &cpuset)) xu_trace_i("get cpu core: %d", i);
            }
        }
    }
    else
        xu_trace_i("set cpu core failed!");
    return 0;
}
