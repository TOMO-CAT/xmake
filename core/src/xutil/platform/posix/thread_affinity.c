#include "xutil/platform/thread.h"

#include <pthread.h>
#include <string.h>

xu_bool_t xu_thread_setaffinity(xu_thread_ref_t thread, xu_cpuset_ref_t cpuset)
{
    // check
    xu_assert_and_check_return_val(cpuset, xu_false);

    // get thread
    pthread_t pthread = thread ? *((pthread_t*)thread) : pthread_self();

    // set cpu affinity
    xu_int_t  i;
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    for (i = 0; i < XU_CPUSET_SIZE; i++)
    {
        if (XU_CPUSET_ISSET(i, cpuset) && i < CPU_SETSIZE) CPU_SET(i, &cpu_set);
    }
    return pthread_setaffinity_np(pthread, sizeof(cpu_set_t), &cpu_set) == 0;
}
xu_bool_t xu_thread_getaffinity(xu_thread_ref_t thread, xu_cpuset_ref_t cpuset)
{
    // check
    xu_assert_and_check_return_val(cpuset, xu_false);

    // get thread
    pthread_t pthread = thread ? *((pthread_t*)thread) : pthread_self();

    // get cpu affinity
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    if (pthread_getaffinity_np(pthread, sizeof(cpu_set_t), &cpu_set) != 0) return xu_false;

    // save cpuset
    xu_int_t i;
    XU_CPUSET_ZERO(cpuset);
    for (i = 0; i < CPU_SETSIZE; i++)
    {
        if (CPU_ISSET(i, &cpu_set) && i < XU_CPUSET_SIZE) XU_CPUSET_SET(i, cpuset);
    }
    return xu_true;
}
