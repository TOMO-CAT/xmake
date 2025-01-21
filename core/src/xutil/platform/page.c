/* *******************************************************
 *  trace
 * *******************************************************
 */
#define XU_TRACE_MODULE_NAME "page"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/platform/page.h"

#if defined(XU_CONFIG_POSIX_HAVE_GETPAGESIZE) || defined(XU_CONFIG_POSIX_HAVE_SYSCONF)
#    include "xutil/platform/posix/page.c"
#else
xu_bool_t xu_page_init() { return xu_true; }
xu_void_t xu_page_exit() {}
xu_size_t xu_page_size()
{
    // default: 4KB
    return 4096;
}
#endif
