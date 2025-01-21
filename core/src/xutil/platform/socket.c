/* *******************************************************
 * trace
 * *******************************************************
 */

#define XU_TRACE_MODULE_NAME "platform_socket"
#define XU_TRACE_MODULE_DEBUG (1)

#include "xutil/platform/socket.h"

/* *******************************************************
 * declaration
 * *******************************************************
 */

xu_long_t xu_socket_wait_impl(xu_socket_ref_t sock, xu_size_t events, xu_long_t timeout);

/* *******************************************************
 * implementation
 * *******************************************************
 */

#if defined(XU_CONFIG_POSIX_HAVE_SOCKET)
#    include "posix/socket.c"
#else
xu_bool_t xu_socket_init_env()
{
    // ok
    return xu_true;
}
xu_void_t       xu_socket_exit_env() {}
xu_socket_ref_t xu_socket_init(xu_size_t type, xu_size_t family)
{
    xu_trace_noimpl();
    return xu_null;
}
xu_bool_t xu_socket_pair(xu_size_t type, xu_socket_ref_t pair[2])
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_socket_ctrl(xu_socket_ref_t sock, xu_size_t ctrl, ...)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_long_t xu_socket_connect(xu_socket_ref_t sock, xu_ipaddr_ref_t addr)
{
    xu_trace_noimpl();
    return -1;
}
xu_bool_t xu_socket_bind(xu_socket_ref_t sock, xu_ipaddr_ref_t addr)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_socket_listen(xu_socket_ref_t sock, xu_size_t backlog)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_socket_ref_t xu_socket_accept(xu_socket_ref_t sock, xu_ipaddr_ref_t addr)
{
    xu_trace_noimpl();
    return xu_null;
}
xu_bool_t xu_socket_local(xu_socket_ref_t sock, xu_ipaddr_ref_t addr)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_socket_kill(xu_socket_ref_t sock, xu_size_t mode)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_bool_t xu_socket_exit(xu_socket_ref_t sock)
{
    xu_trace_noimpl();
    return xu_false;
}
xu_long_t xu_socket_recv(xu_socket_ref_t sock, xu_byte_t* data, xu_size_t size)
{
    xu_trace_noimpl();
    return -1;
}
xu_long_t xu_socket_send(xu_socket_ref_t sock, xu_byte_t const* data, xu_size_t size)
{
    xu_trace_noimpl();
    return -1;
}
xu_long_t xu_socket_recvv(xu_socket_ref_t sock, xu_iovec_t const* list, xu_size_t size)
{
    xu_trace_noimpl();
    return -1;
}
xu_long_t xu_socket_sendv(xu_socket_ref_t sock, xu_iovec_t const* list, xu_size_t size)
{
    xu_trace_noimpl();
    return -1;
}
xu_hong_t xu_socket_sendf(xu_socket_ref_t sock, xu_file_ref_t file, xu_hize_t offset, xu_hize_t size)
{
    xu_trace_noimpl();
    return -1;
}
xu_long_t xu_socket_urecv(xu_socket_ref_t sock, xu_ipaddr_ref_t addr, xu_byte_t* data, xu_size_t size)
{
    xu_trace_noimpl();
    return -1;
}
xu_long_t xu_socket_usend(xu_socket_ref_t sock, xu_ipaddr_ref_t addr, xu_byte_t const* data, xu_size_t size)
{
    xu_trace_noimpl();
    return -1;
}
xu_long_t xu_socket_urecvv(xu_socket_ref_t sock, xu_ipaddr_ref_t addr, xu_iovec_t const* list, xu_size_t size)
{
    xu_trace_noimpl();
    return -1;
}
xu_long_t xu_socket_usendv(xu_socket_ref_t sock, xu_ipaddr_ref_t addr, xu_iovec_t const* list, xu_size_t size)
{
    xu_trace_noimpl();
    return -1;
}
#endif

#if defined(XU_CONFIG_POSIX_HAVE_POLL) &&                                                                              \
    !defined(XU_CONFIG_OS_MACOSX) /* poll(fifo) exists bug on macosx, @see demo/platform/named_pipe.c */
#    include "xutil/platform/posix/socket_poll.c"
#elif defined(XU_CONFIG_POSIX_HAVE_SELECT)
#    include "xutil/platform/posix/socket_select.c"
#else
xu_long_t xu_socket_wait_impl(xu_socket_ref_t sock, xu_size_t events, xu_long_t timeout)
{
    xu_trace_noimpl();
    return -1;
}
#endif
xu_long_t xu_socket_wait(xu_socket_ref_t sock, xu_size_t events, xu_long_t timeout)
{
    // poll it directly if timeout is zero
    if (!timeout) return xu_socket_wait_impl(sock, events, 0);

#if defined(XU_CONFIG_MODULE_HAVE_COROUTINE) && !defined(XU_CONFIG_MICRO_ENABLE)
    // attempt to wait it in coroutine
    if (xu_coroutine_self())
    {
        xu_poller_object_t object;
        object.type     = XU_POLLER_OBJECT_SOCK;
        object.ref.sock = sock;
        return xu_coroutine_waitio(&object, events, timeout);
    }
#endif
    return xu_socket_wait_impl(sock, events, timeout);
}

xu_bool_t xu_socket_brecv(xu_socket_ref_t sock, xu_byte_t* data, xu_size_t size)
{
    // recv data
    xu_size_t recv = 0;
    xu_long_t wait = 0;
    while (recv < size)
    {
        // recv it
        xu_long_t real = xu_socket_recv(sock, data + recv, size - recv);

        // has data?
        if (real > 0)
        {
            recv += real;
            wait = 0;
        }
        // no data? wait it
        else if (!real && !wait)
        {
            // wait it
            wait = xu_socket_wait(sock, XU_SOCKET_EVENT_RECV, -1);
            xu_check_break(wait > 0);
        }
        // failed or end?
        else
            break;
    }
    return recv == size;
}
xu_bool_t xu_socket_bsend(xu_socket_ref_t sock, xu_byte_t const* data, xu_size_t size)
{
    // send data
    xu_size_t send = 0;
    xu_long_t wait = 0;
    while (send < size)
    {
        // send it
        xu_long_t real = xu_socket_send(sock, data + send, size - send);

        // has data?
        if (real > 0)
        {
            send += real;
            wait = 0;
        }
        // no data? wait it
        else if (!real && !wait)
        {
            // wait it
            wait = xu_socket_wait(sock, XU_SOCKET_EVENT_SEND, -1);
            xu_check_break(wait > 0);
        }
        // failed or end?
        else
            break;
    }
    return send == size;
}
