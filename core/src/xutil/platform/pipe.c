/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "pipe"
#define XU_TRACE_MODULE_DEBUG (1)

#include "xutil/platform/pipe.h"

/* *******************************************************
 * implementation
 */
#if defined(XU_CONFIG_POSIX_HAVE_PIPE) || defined(XU_CONFIG_POSIX_HAVE_PIPE2) || defined(XU_CONFIG_POSIX_HAVE_MKFIFO)
#    include "xutil/platform/posix/pipe.c"
#else
xu_pipe_file_ref_t xu_pipe_file_init(xu_char_t const* name, xu_size_t mode, xu_size_t buffer_size)
{
    xu_trace_noimpl();
    return xu_null;
}
xu_bool_t xu_pipe_file_init_pair(xu_pipe_file_ref_t pair[2], xu_size_t mode[2], xu_size_t buffer_size)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_pipe_file_exit(xu_pipe_file_ref_t file)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_long_t xu_pipe_file_read(xu_pipe_file_ref_t file, xu_byte_t* data, xu_size_t size)
{
    xu_trace_noimpl();
    return -1;
}
xu_long_t xu_pipe_file_write(xu_pipe_file_ref_t file, xu_byte_t const* data, xu_size_t size)
{
    xu_trace_noimpl();
    return -1;
}
xu_long_t xu_pipe_file_wait(xu_pipe_file_ref_t file, xu_size_t events, xu_long_t timeout)
{
    xu_trace_noimpl();
    return -1;
}
xu_long_t xu_pipe_file_connect(xu_pipe_file_ref_t file)
{
    xu_trace_noimpl();
    return -1;
}
#endif

xu_bool_t xu_pipe_file_bread(xu_pipe_file_ref_t file, xu_byte_t* data, xu_size_t size)
{
    // read data
    xu_size_t read = 0;
    xu_long_t wait = 0;
    while (read < size)
    {
        // read it
        xu_long_t real = xu_pipe_file_read(file, data + read, xu_min(size - read, 8192));

        // has data?
        if (real > 0)
        {
            read += real;
            wait = 0;
        }
        // no data? wait it
        else if (!real && !wait)
        {
            // wait it
            wait = xu_pipe_file_wait(file, XU_PIPE_EVENT_READ, -1);
            xu_check_break(wait > 0);
        }
        // failed or end?
        else
            break;
    }
    return read == size;
}
xu_bool_t xu_pipe_file_bwrit(xu_pipe_file_ref_t file, xu_byte_t const* data, xu_size_t size)
{
    // writ data
    xu_size_t writ = 0;
    xu_long_t wait = 0;
    while (writ < size)
    {
        // write it
        xu_long_t real = xu_pipe_file_write(file, data + writ, xu_min(size - writ, 8192));

        // has data?
        if (real > 0)
        {
            writ += real;
            wait = 0;
        }
        // no data? wait it
        else if (!real && !wait)
        {
            // wait it
            wait = xu_pipe_file_wait(file, XU_PIPE_EVENT_WRIT, -1);
            xu_check_break(wait > 0);
        }
        // failed or end?
        else
            break;
    }
    return writ == size;
}
