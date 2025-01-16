#include "xutil/platform/directory.h"

#if defined(XU_CONFIG_POSIX_HAVE_OPENDIR)
#    include "xutil/platform/posix/directory.c"
#else
xu_bool_t xu_directory_create(xu_char_t const* path)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_directory_remove(xu_char_t const* path)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_size_t xu_directory_home(xu_char_t* path, xu_size_t maxn)
{
    xu_trace_noimpl();
    return 0;
}
xu_size_t xu_directory_current(xu_char_t* path, xu_size_t maxn)
{
    xu_trace_noimpl();
    return 0;
}
xu_bool_t xu_directory_current_set(xu_char_t const* path)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_size_t xu_directory_temporary(xu_char_t* path, xu_size_t maxn)
{
    xu_trace_noimpl();
    return 0;
}
xu_void_t xu_directory_walk(xu_char_t const* path, xu_long_t recursion, xu_bool_t prefix, xu_directory_walk_func_t func,
                            xu_cpointer_t priv)
{
    xu_trace_noimpl();
}
xu_bool_t xu_directory_copy(xu_char_t const* path, xu_char_t const* dest, xu_size_t flags)
{
    xu_trace_noimpl();
    return xu_false;
}
#endif
