#include "xutil/platform/pipe.h"
#include "xutil/libc/string/string.h"
#include "xutil/platform/directory.h"
#include "xutil/platform/path.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* *******************************************************
 * macros
 */

// fd to pipe file
#define xu_fd2pipefile(fd) ((fd) >= 0 ? (xu_pipe_file_ref_t)((xu_long_t)(fd) + 1) : xu_null)

// pipe file to fd
#define xu_pipefile2fd(file) (xu_int_t)((file) ? (((xu_long_t)(file)) - 1) : -1)

/* *******************************************************
 * declaration
 */

xu_long_t xu_socket_wait_impl(xu_socket_ref_t sock, xu_size_t events, xu_long_t timeout);

/* *******************************************************
 * private implementation
 */

// get pipe file name
static __xu_inline__ xu_char_t const* xu_pipe_file_name(xu_char_t const* name, xu_char_t* data, xu_size_t maxn)
{
    xu_size_t size = xu_directory_temporary(data, maxn);
    if (size && size < maxn)
    {
        if (data[size - 1] != '/') data[size++] = '/';
        if (size < maxn)
        {
            xu_strlcpy(data + size, name, maxn - size);
            return data;
        }
    }
    return xu_null;
}

/* *******************************************************
 * implementation
 */

#ifdef XU_CONFIG_POSIX_HAVE_MKFIFO
xu_pipe_file_ref_t xu_pipe_file_init(xu_char_t const* name, xu_size_t mode, xu_size_t buffer_size)
{
    // check
    xu_assert_and_check_return_val(name, xu_null);

    xu_bool_t ok = xu_false;
    xu_int_t  fd = -1;
    do
    {
        // get pipe name
        xu_char_t        buffer[XU_PATH_MAXN];
        xu_char_t const* pipename = xu_pipe_file_name(name, buffer, xu_arrayn(buffer));
        xu_assert_and_check_break(pipename);

        // this pipe is not exists? we create it first
        if (access(pipename, F_OK) != 0)
        {
            // 0644: -rw-r--r--
            if (mkfifo(pipename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) != 0) break;
        }

        // init flags
        xu_size_t flags = 0;
        if (mode & XU_PIPE_MODE_RO)
            flags |= O_RDONLY;
        else if (mode & XU_PIPE_MODE_WO)
            flags |= O_WRONLY;
        xu_assert_and_check_break(flags);

        // open pipe file
        fd = open(pipename, flags);
        xu_assert_and_check_break(fd >= 0);

        // set block mode
        if (mode & XU_PIPE_MODE_BLOCK)
            fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NONBLOCK);
        else
            fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

        // ok
        ok = xu_true;

    } while (0);
    return ok ? xu_fd2pipefile(fd) : xu_null;
}
#else
xu_pipe_file_ref_t xu_pipe_file_init(xu_char_t const* name, xu_size_t mode, xu_size_t buffer_size)
{
    xu_trace_noimpl();
    return xu_null;
}
#endif

#if defined(XU_CONFIG_POSIX_HAVE_PIPE) || defined(XU_CONFIG_POSIX_HAVE_PIPE2)
xu_bool_t xu_pipe_file_init_pair(xu_pipe_file_ref_t pair[2], xu_size_t mode[2], xu_size_t buffer_size)
{
    // check
    xu_assert_and_check_return_val(pair, xu_false);

    xu_int_t  pipefd[2] = {0};
    xu_bool_t ok        = xu_false;
    do
    {
        // create pipe fd pair
#    ifdef XU_CONFIG_POSIX_HAVE_PIPE2
        if (pipe2(pipefd, O_NONBLOCK) == -1) break;
#    else
        if (pipe(pipefd) == -1) break;
#    endif

        // set block mode (default: non-block mode)
        xu_size_t mode0 = mode ? mode[0] : 0;
        xu_size_t mode1 = mode ? mode[1] : 0;
        if (mode0 & XU_PIPE_MODE_BLOCK)
            fcntl(pipefd[0], F_SETFL, fcntl(pipefd[0], F_GETFL) & ~O_NONBLOCK);
        else
            fcntl(pipefd[0], F_SETFL, fcntl(pipefd[0], F_GETFL) | O_NONBLOCK);

        if (mode1 & XU_PIPE_MODE_BLOCK)
            fcntl(pipefd[1], F_SETFL, fcntl(pipefd[1], F_GETFL) & ~O_NONBLOCK);
        else
            fcntl(pipefd[1], F_SETFL, fcntl(pipefd[1], F_GETFL) | O_NONBLOCK);

        // save to file pair
        pair[0] = xu_fd2pipefile(pipefd[0]);
        pair[1] = xu_fd2pipefile(pipefd[1]);

        // ok
        ok = xu_true;

    } while (0);
    return ok;
}
#else
xu_bool_t xu_pipe_file_init_pair(xu_pipe_file_ref_t pair[2], xu_size_t mode[2], xu_size_t buffer_size)
{
    xu_trace_noimpl();
    return xu_false;
}
#endif
xu_bool_t xu_pipe_file_exit(xu_pipe_file_ref_t file)
{
    // check
    xu_assert_and_check_return_val(file, xu_false);

    // trace
    xu_trace_d("close: %p", file);

#ifdef XU_CONFIG_MODULE_HAVE_COROUTINE
    // attempt to cancel waiting from coroutine first
    xu_pointer_t       scheduler_io = xu_null;
    xu_poller_object_t object;
    object.type     = XU_POLLER_OBJECT_PIPE;
    object.ref.pipe = file;
#    ifndef XU_CONFIG_MICRO_ENABLE
    if ((scheduler_io = xu_co_scheduler_io_self()) &&
        xu_co_scheduler_io_cancel((xu_co_scheduler_io_ref_t)scheduler_io, &object))
    {
    }
    else
#    endif
        if ((scheduler_io = xu_lo_scheduler_io_self()) &&
            xu_lo_scheduler_io_cancel((xu_lo_scheduler_io_ref_t)scheduler_io, &object))
    {
    }
#endif

    // close it
    xu_bool_t ok = !close(xu_pipefile2fd(file));
    if (!ok)
    {
        // trace
        xu_trace_e("close: %p failed, errno: %d", file, errno);
    }
    return ok;
}
xu_long_t xu_pipe_file_connect(xu_pipe_file_ref_t file) { return 1; }
xu_long_t xu_pipe_file_read(xu_pipe_file_ref_t file, xu_byte_t* data, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(file && data, -1);
    xu_check_return_val(size, 0);

    // read
    xu_long_t real = read(xu_pipefile2fd(file), data, (xu_int_t)size);

    // trace
    xu_trace_d("read: %p %lu => %ld, errno: %d", file, size, real, errno);

    // ok?
    if (real >= 0) return real;

    // continue?
    if (errno == EINTR || errno == EAGAIN) return 0;

    // error
    return -1;
}
xu_long_t xu_pipe_file_write(xu_pipe_file_ref_t file, xu_byte_t const* data, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(file && data, -1);
    xu_check_return_val(size, 0);

    // write
    xu_long_t real = write(xu_pipefile2fd(file), data, (xu_int_t)size);

    // trace
    xu_trace_d("write: %p %lu => %ld, errno: %d", file, size, real, errno);

    // ok?
    if (real >= 0) return real;

    // continue?
    if (errno == EINTR || errno == EAGAIN) return 0;

    // error
    return -1;
}
xu_long_t xu_pipe_file_wait(xu_pipe_file_ref_t file, xu_size_t events, xu_long_t timeout)
{
#if defined(XU_CONFIG_MODULE_HAVE_COROUTINE) && !defined(XU_CONFIG_MICRO_ENABLE)
    // attempt to wait it in coroutine if timeout is non-zero
    if (timeout && xu_coroutine_self())
    {
        xu_poller_object_t object;
        object.type     = XU_POLLER_OBJECT_PIPE;
        object.ref.pipe = file;
        return xu_coroutine_waitio(&object, events, timeout);
    }
#endif
    // we use poll/select to wait pipe/fd events
    return xu_socket_wait_impl((xu_socket_ref_t)file, events, timeout);
}
