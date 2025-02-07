#define XU_TRACE_MODULE_NAME                "fwatcher"
#define XU_TRACE_MODULE_DEBUG               (1)

/* *******************************************************
 * includes
 */
#include "xutil/platform/fwatcher.h"

/* *******************************************************
 * implementation
 */
#if defined(XU_CONFIG_LINUX_HAVE_INOTIFY_INIT)
#   include "xutil/platform/linux/fwatcher_inotify.c"
#elif defined(XU_CONFIG_OS_MACOSX)
#   include "xutil/platform/mach/fwatcher_fsevent.c"
#elif defined(XU_CONFIG_OS_BSD)
#   include "xutil/platform/bsd/fwatcher_kqueue.c"
#else
xu_fwatcher_ref_t xu_fwatcher_init()
{
    xu_trace_noimpl();
    return xu_null;
}

xu_void_t xu_fwatcher_exit(xu_fwatcher_ref_t self)
{
    xu_trace_noimpl();
}

xu_bool_t xu_fwatcher_add(xu_fwatcher_ref_t self, xu_char_t const* watchdir, xu_bool_t recursion)
{
    xu_trace_noimpl();
    return xu_false;
}

xu_bool_t xu_fwatcher_remove(xu_fwatcher_ref_t self, xu_char_t const* watchdir)
{
    xu_trace_noimpl();
    return xu_false;
}

xu_void_t xu_fwatcher_spak(xu_fwatcher_ref_t self)
{
    xu_trace_noimpl();
}

xu_long_t xu_fwatcher_wait(xu_fwatcher_ref_t self, xu_fwatcher_event_t* event, xu_long_t timeout)
{
    xu_trace_noimpl();
    return -1;
}

#endif
