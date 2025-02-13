#include "xutil/platform/hostname.h"

/* *******************************************************
 * implementation
 */
#if defined(XU_CONFIG_POSIX_HAVE_GETHOSTNAME)
#    include <unistd.h>
xu_bool_t xu_hostname(xu_char_t* name, xu_size_t size) { return !gethostname(name, size) ? xu_true : xu_false; }

#else
xu_bool_t xu_hostname(xu_char_t* name, xu_size_t size)
{
    xu_trace_noimpl();
    return xu_false;
}
#endif
