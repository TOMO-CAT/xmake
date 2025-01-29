#include "xutil/platform/filelock.h"

/* *******************************************************
 * implementation
 */
#if defined(XU_CONFIG_BSD_HAVE_FLOCK) && !defined(XU_CONFIG_OS_HAIKU)
#    include "xutil/platform/bsd/filelock.c"
#elif defined(XU_CONFIG_POSIX_HAVE_FCNTL)
#    include "xutil/platform/posix/filelock.c"
#else
xu_filelock_ref_t xu_filelock_init(xu_file_ref_t file)
{
    xu_trace_noimpl();
    return xu_null;
}
xu_filelock_ref_t xu_filelock_init_from_path(xu_char_t const* path, xu_size_t mode)
{
    xu_trace_noimpl();
    return xu_null;
}
xu_void_t xu_filelock_exit(xu_filelock_ref_t lock) { xu_trace_noimpl(); }
xu_bool_t xu_filelock_enter(xu_filelock_ref_t lock, xu_size_t mode)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_filelock_enter_try(xu_filelock_ref_t lock, xu_size_t mode)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_filelock_leave(xu_filelock_ref_t lock)
{
    xu_trace_noimpl();
    return xu_false;
}
#endif
