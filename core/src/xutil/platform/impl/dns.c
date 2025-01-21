/* *******************************************************
 *  trace
 * *******************************************************
 */

#define XU_TRACE_MODULE_NAME "dns"
#define XU_TRACE_MODULE_DEBUG (1)

#include "xutil/platform/impl/dns.h"

#if defined(XU_CONFIG_OS_MACOSX) || defined(XU_CONFIG_OS_IOS)
#    include "../mach/dns.c"
#elif defined(XU_CONFIG_OS_ANDROID)
#    include "../android/dns.c"
#else
#    include "xutil/platform/unix/dns.c"
#endif
