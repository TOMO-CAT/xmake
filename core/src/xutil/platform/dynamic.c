#include "xutil/platform/dynamic.h"

#if defined(XU_CONFIG_POSIX_HAVE_DLOPEN)
#    include "xutil/platform/posix/dynamic.c"
#else
xu_dynamic_ref_t xu_dynamic_init(xu_char_t const* name)
{
    xu_trace_noimpl();
    return xu_null;
}
xu_void_t    xu_dynamic_exit(xu_dynamic_ref_t dynamic) { xu_trace_noimpl(); }
xu_pointer_t xu_dynamic_func(xu_dynamic_ref_t dynamic, xu_char_t const* name)
{
    xu_trace_noimpl();
    return xu_null;
}
xu_pointer_t xu_dynamic_pvar(xu_dynamic_ref_t dynamic, xu_char_t const* name)
{
    xu_trace_noimpl();
    return xu_null;
}
#endif
