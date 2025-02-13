#if defined(XU_CONFIG_OS_MACOSX)
#    include "xutil/platform/sched.h"
#    include <mach/mach.h>
#    include <mach/mach_vm.h>

/* *******************************************************
 * implementation
 */
xu_bool_t xu_sched_setaffinity(xu_size_t pid, xu_cpuset_ref_t cpuset)
{
    // check
    xu_assert_and_check_return_val(cpuset, xu_false);

    // get the first cpu core index
    xu_int_t core = XU_CPUSET_FFS(cpuset);

    // trace
    xu_trace_d("setaffinity: core(%d) to pid(%zu)", core, pid);

    // get port
    mach_port_name_t port;
    if (pid)
    {
        if (task_for_pid(mach_task_self(), (xu_int_t)pid, &port) != KERN_SUCCESS)
        {
            xu_trace_e("cannot get port for the gived pid(%zu)!", pid);
            return xu_false;
        }
    }
    else
        port = mach_task_self();

    // get all threads
    thread_act_t*          threads      = xu_null;
    mach_msg_type_number_t thread_count = 0;
    if (task_threads(port, &threads, &thread_count) != KERN_SUCCESS)
    {
        xu_trace_e("cannot get task threads for the gived pid(%lu)!", pid);
        return xu_false;
    }
    xu_assert_and_check_return_val(threads && thread_count > 0, xu_false);

    // suspend threads first
    xu_size_t i;
    thread_t  thread_self = mach_thread_self();
    mach_port_deallocate(mach_task_self(), thread_self);
    for (i = 0; i < thread_count; i++)
    {
        if (threads[i] != MACH_PORT_NULL && threads[i] != thread_self) thread_suspend(threads[i]);
    }

    // set affinity for all threads
    xu_size_t                     ok = xu_true;
    thread_affinity_policy_data_t policy;
    for (i = 0; i < thread_count; i++)
    {
        if (threads[i] != MACH_PORT_NULL)
        {
            // TODO only set the first thread successfully now.
            policy.affinity_tag = core;
            if (thread_policy_set(threads[i], THREAD_AFFINITY_POLICY, (thread_policy_t)&policy,
                                  THREAD_AFFINITY_POLICY_COUNT) != KERN_SUCCESS)
                ok = xu_false;
        }
    }

    // resume threads
    for (i = 0; i < thread_count; i++)
    {
        if (threads[i] != MACH_PORT_NULL && threads[i] != thread_self) thread_suspend(threads[i]);
    }

    // free threads
    for (i = 0; i < thread_count; i++)
    {
        if (threads[i] != MACH_PORT_NULL) mach_port_deallocate(port, threads[i]);
    }
    mach_vm_deallocate(port, (mach_vm_address_t)threads, thread_count * sizeof(*threads));
    return ok;
}
xu_bool_t xu_sched_getaffinity(xu_size_t pid, xu_cpuset_ref_t cpuset)
{
    // check
    xu_assert_and_check_return_val(cpuset, xu_false);

    // get port
    mach_port_name_t port;
    if (pid)
    {
        if (task_for_pid(mach_task_self(), (xu_int_t)pid, &port) != KERN_SUCCESS)
        {
            xu_trace_e("cannot get port for the gived pid(%lu)!", pid);
            return xu_false;
        }
    }
    else
        port = mach_task_self();

    // get all threads
    thread_act_t*          threads      = xu_null;
    mach_msg_type_number_t thread_count = 0;
    if (task_threads(port, &threads, &thread_count) != KERN_SUCCESS)
    {
        xu_trace_e("cannot get task threads for the gived pid(%lu)!", pid);
        return xu_false;
    }
    xu_assert_and_check_return_val(threads && thread_count > 0, xu_false);

    // suspend threads first
    xu_size_t i;
    thread_t  thread_self = mach_thread_self();
    mach_port_deallocate(mach_task_self(), thread_self);
    for (i = 0; i < thread_count; i++)
    {
        if (threads[i] != MACH_PORT_NULL && threads[i] != thread_self) thread_suspend(threads[i]);
    }

    // set affinity for all threads
    xu_size_t ok = xu_true;
    XU_CPUSET_ZERO(cpuset);
    for (i = 0; i < thread_count; i++)
    {
        if (threads[i] != MACH_PORT_NULL)
        {
            boolean_t                     get_default = xu_false;
            mach_msg_type_number_t        count       = THREAD_AFFINITY_POLICY_COUNT;
            thread_affinity_policy_data_t policy;
            policy.affinity_tag = 0;
            if (thread_policy_get(threads[i], THREAD_AFFINITY_POLICY, (thread_policy_t)&policy, &count, &get_default) ==
                KERN_SUCCESS)
            {
                XU_CPUSET_SET(policy.affinity_tag, cpuset);
            }
            else
                ok = xu_false;
        }
    }

    // resume threads
    for (i = 0; i < thread_count; i++)
    {
        if (threads[i] != MACH_PORT_NULL && threads[i] != thread_self) thread_suspend(threads[i]);
    }

    // free threads
    for (i = 0; i < thread_count; i++)
    {
        if (threads[i] != MACH_PORT_NULL) mach_port_deallocate(port, threads[i]);
    }
    mach_vm_deallocate(port, (mach_vm_address_t)threads, thread_count * sizeof(*threads));
    return ok;
}
#endif
