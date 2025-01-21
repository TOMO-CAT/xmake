/* *******************************************************
 * trace
 * *******************************************************
 */
#define XU_TRACE_MODULE_NAME "process"
#define XU_TRACE_MODULE_DEBUG (1)

#include "xutil/platform/process.h"

/* *******************************************************
 * declaration
 * *******************************************************
 */

xu_bool_t xu_process_group_init();
xu_void_t xu_process_group_exit();

/* *******************************************************
 * implementation
 * *******************************************************
 */

#if defined(XU_CONFIG_POSIX_HAVE_WAITPID) && defined(XU_CONFIG_POSIX_HAVE_POSIX_SPAWNP)
#    include "xutil/platform/posix/process.c"
#elif defined(XU_CONFIG_POSIX_HAVE_WAITPID) &&                                                                         \
    (defined(XU_CONFIG_POSIX_HAVE_FORK) || defined(XU_CONFIG_POSIX_HAVE_VFORK)) &&                                     \
    (defined(XU_CONFIG_POSIX_HAVE_EXECVP) || defined(XU_CONFIG_POSIX_HAVE_EXECVPE))
#    include "xutil/platform/posix/process.c"
#else
xu_process_ref_t xu_process_init(xu_char_t const* pathname, xu_char_t const* argv[], xu_process_attr_ref_t attr)
{
    xu_trace_noimpl();
    return xu_null;
}
xu_process_ref_t xu_process_init_cmd(xu_char_t const* cmd, xu_process_attr_ref_t attr)
{
    xu_trace_noimpl();
    return xu_null;
}
xu_void_t     xu_process_exit(xu_process_ref_t self) { xu_trace_noimpl(); }
xu_void_t     xu_process_kill(xu_process_ref_t self) { xu_trace_noimpl(); }
xu_cpointer_t xu_process_priv(xu_process_ref_t self)
{
    xu_trace_noimpl();
    return xu_null;
}
xu_void_t xu_process_priv_set(xu_process_ref_t process, xu_cpointer_t priv) { xu_trace_noimpl(); }
xu_void_t xu_process_resume(xu_process_ref_t self) { xu_trace_noimpl(); }
xu_void_t xu_process_suspend(xu_process_ref_t self) { xu_trace_noimpl(); }
xu_long_t xu_process_wait(xu_process_ref_t self, xu_long_t* pstatus, xu_long_t timeout)
{
    xu_trace_noimpl();
    return -1;
}
xu_long_t xu_process_waitlist(xu_process_ref_t const* processes, xu_process_waitinfo_ref_t infolist, xu_size_t infomaxn,
                              xu_long_t timeout)
{
    xu_trace_noimpl();
    return -1;
}
xu_bool_t xu_process_group_init() { return xu_true; }
xu_void_t xu_process_group_exit() {}
#endif
xu_long_t xu_process_run(xu_char_t const* pathname, xu_char_t const* argv[], xu_process_attr_ref_t attr)
{
    // remove suspend
    if (attr) attr->flags &= ~XU_PROCESS_FLAG_SUSPEND;

    // init process
    xu_long_t        ok      = -1;
    xu_process_ref_t process = xu_process_init(pathname, argv, attr);
    if (process)
    {
        // wait process
        xu_long_t status = 0;
        if (xu_process_wait(process, &status, -1) > 0) ok = status;

        // exit process
        xu_process_exit(process);
    }

    // ok?
    return ok;
}
xu_long_t xu_process_run_cmd(xu_char_t const* cmd, xu_process_attr_ref_t attr)
{
    // remove suspend
    if (attr) attr->flags &= ~XU_PROCESS_FLAG_SUSPEND;

    // init process
    xu_long_t        ok      = -1;
    xu_process_ref_t process = xu_process_init_cmd(cmd, attr);
    if (process)
    {
        // wait process
        xu_long_t status = 0;
        if (xu_process_wait(process, &status, -1) > 0) ok = status;

        // exit process
        xu_process_exit(process);
    }

    // ok?
    return ok;
}
