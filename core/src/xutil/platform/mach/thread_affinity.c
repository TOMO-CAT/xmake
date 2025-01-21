#include "xutil/platform/thread.h"

#if defined(XU_CONFIG_OS_MACOSX) || defined(XU_CONFIG_OS_IOS)
#    include <mach/mach.h>
#    include <pthread.h>

xu_bool_t xu_thread_setaffinity(xu_thread_ref_t thread, xu_cpuset_ref_t cpuset)
{
    // check
    xu_assert_and_check_return_val(cpuset, xu_false);

    // get mach thread
    pthread_t     pthread     = thread ? *((pthread_t*)thread) : pthread_self();
    thread_port_t mach_thread = pthread_mach_thread_np(pthread);
    xu_assert_and_check_return_val(mach_thread != MACH_PORT_NULL, xu_false);

    // get the first cpu core index
    xu_int_t core = XU_CPUSET_FFS(cpuset);

    // set thread affinity
    thread_affinity_policy_data_t policy;
    policy.affinity_tag = core;
    return thread_policy_set(mach_thread, THREAD_AFFINITY_POLICY, (thread_policy_t)&policy,
                             THREAD_AFFINITY_POLICY_COUNT) == KERN_SUCCESS;
}
xu_bool_t xu_thread_getaffinity(xu_thread_ref_t thread, xu_cpuset_ref_t cpuset)
{
    // check
    xu_assert_and_check_return_val(cpuset, xu_false);

    // get mach thread
    pthread_t     pthread     = thread ? *((pthread_t*)thread) : pthread_self();
    thread_port_t mach_thread = pthread_mach_thread_np(pthread);
    xu_assert_and_check_return_val(mach_thread != MACH_PORT_NULL, xu_false);

    // get thread affinity
    boolean_t                     get_default = xu_false;
    mach_msg_type_number_t        count       = THREAD_AFFINITY_POLICY_COUNT;
    thread_affinity_policy_data_t policy;
    policy.affinity_tag = 0;
    if (thread_policy_get(mach_thread, THREAD_AFFINITY_POLICY, (thread_policy_t)&policy, &count, &get_default) !=
        KERN_SUCCESS)
        return xu_false;

    // save to cpuset
    XU_CPUSET_ZERO(cpuset);
    XU_CPUSET_SET(policy.affinity_tag, cpuset);
    return xu_true;
}
#endif
