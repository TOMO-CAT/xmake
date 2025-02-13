/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "platform_sched"
#define XU_TRACE_MODULE_DEBUG (1)

/* *******************************************************
 * includes
 */
#include "xutil/platform/sched.h"

/* *******************************************************
 * implementation
 */
#if defined(XU_CONFIG_OS_MACOSX)
#    include "xutil/platform/mach/sched_affinity.c"
#elif defined(XU_CONFIG_POSIX_HAVE_SCHED_SETAFFINITY)
#    include "xutil/platform/posix/sched_affinity.c"
#else
xu_bool_t xu_sched_setaffinity(xu_size_t pid, xu_cpuset_ref_t cpuset)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_sched_getaffinity(xu_size_t pid, xu_cpuset_ref_t cpuset)
{
    xu_trace_noimpl();
    return xu_false;
}
#endif
