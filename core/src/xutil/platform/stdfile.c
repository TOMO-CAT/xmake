/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME                "platform_stdfile"
#define XU_TRACE_MODULE_DEBUG               (0)

/* *******************************************************
 * includes
 */
#include "xutil/platform/stdfile.h"
#include "xutil/utils/utils.h"
#include "xutil/utils/singleton.h"
#if defined(XU_CONFIG_POSIX_HAVE_SELECT)
#   include <unistd.h>
#   include <sys/select.h>
#   include <errno.h>
#endif

/* *******************************************************
 * instance implementation
 */
static xu_handle_t xu_stdfile_instance_init(xu_cpointer_t* ppriv)
{
    xu_size_t* ptype = (xu_size_t*)ppriv;
    return ptype? (xu_handle_t)xu_stdfile_init(*ptype) : xu_null;
}
static xu_void_t xu_stdfile_instance_exit(xu_handle_t stdfile, xu_cpointer_t priv)
{
    xu_stdfile_exit((xu_stdfile_ref_t)stdfile);
}

/* *******************************************************
 * implementation
 */
xu_stdfile_ref_t xu_stdfile_input()
{
    return (xu_stdfile_ref_t)xu_singleton_instance(XU_SINGLETON_TYPE_STDFILE_STDIN, xu_stdfile_instance_init, xu_stdfile_instance_exit, xu_null, xu_u2p(XU_STDFILE_TYPE_STDIN));
}
xu_stdfile_ref_t xu_stdfile_output()
{
    return (xu_stdfile_ref_t)xu_singleton_instance(XU_SINGLETON_TYPE_STDFILE_STDOUT, xu_stdfile_instance_init, xu_stdfile_instance_exit, xu_null, xu_u2p(XU_STDFILE_TYPE_STDOUT));
}
xu_stdfile_ref_t xu_stdfile_error()
{
    return (xu_stdfile_ref_t)xu_singleton_instance(XU_SINGLETON_TYPE_STDFILE_STDERR, xu_stdfile_instance_init, xu_stdfile_instance_exit, xu_null, xu_u2p(XU_STDFILE_TYPE_STDERR));
}

#if defined(XU_CONFIG_LIBC_HAVE_FREAD) && defined(XU_CONFIG_LIBC_HAVE_FWRITE)
#   include "xutil/platform/libc/stdfile.c"
#else
xu_stdfile_ref_t xu_stdfile_init(xu_size_t type)
{
    xu_trace_noimpl();
    return xu_null;
}
xu_void_t xu_stdfile_exit(xu_stdfile_ref_t stdfile)
{
    xu_trace_noimpl();
}
xu_size_t xu_stdfile_type(xu_stdfile_ref_t stdfile)
{
    xu_trace_noimpl();
    return 0;
}
xu_bool_t xu_stdfile_flush(xu_stdfile_ref_t stdfile)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_stdfile_read(xu_stdfile_ref_t file, xu_byte_t* data, xu_size_t size)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_stdfile_writ(xu_stdfile_ref_t file, xu_byte_t const* data, xu_size_t size)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_stdfile_peek(xu_stdfile_ref_t file, xu_char_t* pch)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_stdfile_getc(xu_stdfile_ref_t file, xu_char_t* pch)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_stdfile_putc(xu_stdfile_ref_t file, xu_char_t ch)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_stdfile_gets(xu_stdfile_ref_t file, xu_char_t* str, xu_size_t num)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_stdfile_puts(xu_stdfile_ref_t file, xu_char_t const* str)
{
    xu_trace_noimpl();
    return xu_false;
}
#endif

#if defined(XU_CONFIG_POSIX_HAVE_SELECT)
xu_bool_t xu_stdfile_readable(xu_stdfile_ref_t self)
{
    // check
    xu_stdfile_t* stdfile = (xu_stdfile_t*)self;
    xu_assert_and_check_return_val(stdfile && stdfile->fp, xu_false);
    xu_assert_and_check_return_val(stdfile->type == XU_STDFILE_TYPE_STDIN, xu_false);

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    return select(1, &readfds, xu_null, xu_null, &timeout) > 0;
}
#else
xu_bool_t xu_stdfile_readable(xu_stdfile_ref_t self)
{
    xu_trace_noimpl();
    return xu_false;
}
#endif
