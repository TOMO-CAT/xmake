#include "xutil/platform/socket.h"
#include "xutil/platform/posix/sockaddr.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#ifdef XU_CONFIG_POSIX_HAVE_SENDFILE
#    include <sys/sendfile.h>
#endif

/* *******************************************************
 * macros
 * *******************************************************
 */

// no sigpipe
#if !defined(SO_NOSIGPIPE) && defined(MSG_NOSIGNAL)
#    define SO_NOSIGPIPE MSG_NOSIGNAL
#endif

/* *******************************************************
 * private implementation
 * *******************************************************
 */

static xu_int_t xu_socket_type(xu_size_t type)
{
    // get socket type
    switch ((type >> 8) << 8)
    {
    case XU_SOCKET_TYPE_SOCK_STREAM: return SOCK_STREAM;
    case XU_SOCKET_TYPE_SOCK_DGRAM: return SOCK_DGRAM;
    case XU_SOCKET_TYPE_SOCK_RAW: return SOCK_RAW;
    }

    // failed
    return -1;
}
static xu_int_t xu_socket_proto(xu_size_t type, xu_size_t family)
{
    if (family == XU_IPADDR_FAMILY_UNIX) return 0;
    // get protocal type
    switch (type & 0xff)
    {
    case XU_SOCKET_TYPE_IPPROTO_TCP: return IPPROTO_TCP;
    case XU_SOCKET_TYPE_IPPROTO_UDP: return IPPROTO_UDP;
    case XU_SOCKET_TYPE_IPPROTO_ICMP:
        switch (family)
        {
        case XU_IPADDR_FAMILY_IPV4: return IPPROTO_ICMP;
        case XU_IPADDR_FAMILY_IPV6: return IPPROTO_ICMPV6;
        default: xu_assert(0);
        }
    }

    // failed
    return -1;
}

/* *******************************************************
 * implementation
 * *******************************************************
 */

xu_bool_t xu_socket_init_env()
{
    // ignore sigpipe
#if defined(XU_CONFIG_LIBC_HAVE_SIGNAL) && !defined(XU_CONFIG_OS_ANDROID)
    signal(SIGPIPE, SIG_IGN);
#endif

    // ok
    return xu_true;
}
xu_void_t       xu_socket_exit_env() {}
xu_socket_ref_t xu_socket_init(xu_size_t type, xu_size_t family)
{
    // check
    xu_assert_and_check_return_val(type, xu_null);

    // done
    xu_socket_ref_t sock = xu_null;
    do
    {
        // init socket type and protocol
        xu_int_t t = xu_socket_type(type);
        xu_int_t p = xu_socket_proto(type, family);
        xu_assert_and_check_break(t >= 0 && p >= 0);

        // init socket family
        xu_int_t f = (family == XU_IPADDR_FAMILY_UNIX)   ? AF_UNIX
                     : (family == XU_IPADDR_FAMILY_IPV6) ? AF_INET6
                                                         : AF_INET;

        // sock
        xu_int_t fd = socket(f, t, p);
        xu_assert_and_check_break(fd >= 0);

        // set non-block mode
        fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

        // save socket
        sock = xu_fd2sock(fd);

    } while (0);

    // trace
    xu_trace_d("init: type: %lu, family: %lu, sock: %p", type, family, sock);

    // ok?
    return sock;
}
xu_bool_t xu_socket_pair(xu_size_t type, xu_socket_ref_t pair[2])
{
    // check
    xu_assert_and_check_return_val(type && pair, xu_false);

    // init socket type
    xu_int_t t = xu_socket_type(type);
    xu_assert_and_check_return_val(t >= 0, xu_false);

    // make pair (protocol must be 0)
    xu_int_t fd[2] = {0};
    if (socketpair(AF_LOCAL, t, 0, fd) == -1) return xu_false;

    // non-block
    fcntl(fd[0], F_SETFL, fcntl(fd[0], F_GETFL) | O_NONBLOCK);
    fcntl(fd[1], F_SETFL, fcntl(fd[1], F_GETFL) | O_NONBLOCK);

    // save pair
    pair[0] = xu_fd2sock(fd[0]);
    pair[1] = xu_fd2sock(fd[1]);

    // trace
    xu_trace_d("pair: %p %p", pair[0], pair[1]);

    // ok
    return xu_true;
}
xu_bool_t xu_socket_ctrl(xu_socket_ref_t sock, xu_size_t ctrl, ...)
{
    // check
    xu_assert_and_check_return_val(sock, xu_false);

    // init args
    xu_va_list_t args;
    xu_va_start(args, ctrl);

    // done
    xu_int_t  fd = xu_sock2fd(sock);
    xu_bool_t ok = xu_false;
    switch (ctrl)
    {
    case XU_SOCKET_CTRL_SET_BLOCK:
    {
        // set block
        xu_bool_t is_block = (xu_bool_t)xu_va_arg(args, xu_bool_t);

        // block it?
        if (is_block)
            fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NONBLOCK);
        else
            fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

        // ok
        ok = xu_true;
    }
    break;
    case XU_SOCKET_CTRL_GET_BLOCK:
    {
        // the pis_block
        xu_bool_t* pis_block = (xu_bool_t*)xu_va_arg(args, xu_bool_t*);
        xu_assert_and_check_return_val(pis_block, xu_false);

        // is block?
        *pis_block = (fcntl(fd, F_GETFL) & O_NONBLOCK) ? xu_false : xu_true;

        // ok
        ok = xu_true;
    }
    break;
    case XU_SOCKET_CTRL_SET_TCP_NODELAY:
    {
        // enable the nagle's algorithm
        xu_int_t enable = (xu_int_t)xu_va_arg(args, xu_bool_t);
        if (!setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (xu_char_t*)&enable, sizeof(enable))) ok = xu_true;
    }
    break;
    case XU_SOCKET_CTRL_GET_TCP_NODELAY:
    {
        // the penable
        xu_bool_t* penable = (xu_bool_t*)xu_va_arg(args, xu_bool_t*);
        xu_assert_and_check_return_val(penable, xu_false);

        // the nagle's algorithm is enabled?
        xu_int_t  enable = 0;
        socklen_t size   = sizeof(enable);
        if (!getsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (xu_char_t*)&enable, &size))
        {
            // save it
            *penable = (xu_bool_t)enable;

            // ok
            ok = xu_true;
        }
        else
            *penable = xu_false;
    }
    break;
    case XU_SOCKET_CTRL_SET_RECV_BUFF_SIZE:
    {
        // the buff_size
        xu_size_t buff_size = (xu_size_t)xu_va_arg(args, xu_size_t);

        // set the recv buffer size
        xu_int_t real = (xu_int_t)buff_size;
        if (!setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (xu_char_t*)&real, sizeof(real))) ok = xu_true;
    }
    break;
    case XU_SOCKET_CTRL_GET_RECV_BUFF_SIZE:
    {
        // the pbuff_size
        xu_size_t* pbuff_size = (xu_size_t*)xu_va_arg(args, xu_size_t*);
        xu_assert_and_check_return_val(pbuff_size, xu_false);

        // get the recv buffer size
        xu_int_t  real = 0;
        socklen_t size = sizeof(real);
        if (!getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (xu_char_t*)&real, &size))
        {
            // save it
            *pbuff_size = real;

            // ok
            ok = xu_true;
        }
        else
            *pbuff_size = 0;
    }
    break;
    case XU_SOCKET_CTRL_SET_SEND_BUFF_SIZE:
    {
        // the buff_size
        xu_size_t buff_size = (xu_size_t)xu_va_arg(args, xu_size_t);

        // set the send buffer size
        xu_int_t real = (xu_int_t)buff_size;
        if (!setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (xu_char_t*)&real, sizeof(real))) ok = xu_true;
    }
    break;
    case XU_SOCKET_CTRL_GET_SEND_BUFF_SIZE:
    {
        // the pbuff_size
        xu_size_t* pbuff_size = (xu_size_t*)xu_va_arg(args, xu_size_t*);
        xu_assert_and_check_return_val(pbuff_size, xu_false);

        // get the send buffer size
        xu_int_t  real = 0;
        socklen_t size = sizeof(real);
        if (!getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (xu_char_t*)&real, &size))
        {
            // save it
            *pbuff_size = real;

            // ok
            ok = xu_true;
        }
        else
            *pbuff_size = 0;
    }
    break;
#ifdef SO_NOSIGPIPE
    case XU_SOCKET_CTRL_SET_NOSIGPIPE:
    {
        xu_int_t enable = (xu_int_t)xu_va_arg(args, xu_bool_t);
        if (!setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, (xu_char_t*)&enable, sizeof(enable))) ok = xu_true;
    }
    break;
#endif
#ifdef SO_KEEPALIVE
    case XU_SOCKET_CTRL_SET_KEEPALIVE:
    {
        xu_int_t enable = (xu_int_t)xu_va_arg(args, xu_bool_t);
        if (!setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (xu_char_t*)&enable, sizeof(enable))) ok = xu_true;
    }
    break;
#endif
#ifdef TCP_KEEPINTVL
    case XU_SOCKET_CTRL_SET_TCP_KEEPINTVL:
    {
        xu_int_t intvl = (xu_int_t)xu_va_arg(args, xu_size_t);
        if (!setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, (xu_char_t*)&intvl, sizeof(intvl))) ok = xu_true;
    }
    break;
#endif
    default:
    {
        // trace
        xu_trace_e("unknown socket ctrl: %lu", ctrl);
    }
    break;
    }

    // exit args
    xu_va_end(args);

    // ok?
    return ok;
}
xu_long_t xu_socket_connect(xu_socket_ref_t sock, xu_ipaddr_ref_t addr)
{
    // check
    xu_assert_and_check_return_val(sock && addr, -1);
    xu_assert_and_check_return_val(!xu_ipaddr_is_empty(addr), -1);

    // load addr
    xu_size_t               n = 0;
    struct sockaddr_storage d;
    if (!(n = xu_sockaddr_load(&d, addr))) return -1;

    // connect
    xu_long_t r = connect(xu_sock2fd(sock), (struct sockaddr*)&d, n);

    // trace
    xu_trace_d("connect: %p, %ld, error: %d", sock, r, errno);

    // ok?
    if (!r || errno == EISCONN) return 1;

    // continue?
    if (errno == EINTR || errno == EINPROGRESS || errno == EAGAIN) return 0;

    // error
    return -1;
}
xu_bool_t xu_socket_bind(xu_socket_ref_t sock, xu_ipaddr_ref_t addr)
{
    // check
    xu_assert_and_check_return_val(sock && addr, xu_false);

    // load addr
    xu_int_t                n = 0;
    struct sockaddr_storage d;
    if (!(n = (xu_int_t)xu_sockaddr_load(&d, addr))) return xu_false;

        // reuse addr
#ifdef SO_REUSEADDR
    {
        xu_int_t reuseaddr = 1;
        if (setsockopt(xu_sock2fd(sock), SOL_SOCKET, SO_REUSEADDR, (xu_int_t*)&reuseaddr, sizeof(reuseaddr)) < 0)
        {
            // trace
            xu_trace_e("reuseaddr: failed");
        }
    }
#endif

    // reuse port
#ifdef SO_REUSEPORT
    if (xu_ipaddr_port(addr))
    {
        xu_int_t reuseport = 1;
        if (setsockopt(xu_sock2fd(sock), SOL_SOCKET, SO_REUSEPORT, (xu_int_t*)&reuseport, sizeof(reuseport)) < 0)
        {
            // trace
            xu_trace_e("reuseport: %u failed", xu_ipaddr_port(addr));
        }
    }
#endif

    // only bind ipv6 address
    if (xu_ipaddr_family(addr) == XU_IPADDR_FAMILY_IPV6)
    {
        xu_int_t only_ipv6 = 1;
        if (setsockopt(xu_sock2fd(sock), IPPROTO_IPV6, IPV6_V6ONLY, (xu_int_t*)&only_ipv6, sizeof(only_ipv6)) < 0)
        {
            // trace
            xu_trace_e("set only ipv6 failed");
        }
    }

    // bind
    return !bind(xu_sock2fd(sock), (struct sockaddr*)&d, n);
}
xu_bool_t xu_socket_listen(xu_socket_ref_t sock, xu_size_t backlog)
{
    // check
    xu_assert_and_check_return_val(sock, xu_false);

    // listen
    return (listen(xu_sock2fd(sock), backlog) < 0) ? xu_false : xu_true;
}
xu_socket_ref_t xu_socket_accept(xu_socket_ref_t sock, xu_ipaddr_ref_t addr)
{
    // check
    xu_assert_and_check_return_val(sock, xu_null);

    // done
    struct sockaddr_storage d;
    socklen_t               n  = sizeof(struct sockaddr_in);
    xu_long_t               fd = accept(xu_sock2fd(sock), (struct sockaddr*)&d, &n);

    // no client?
    xu_check_return_val(fd > 0, xu_null);

    // non-block
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

    /* disable the nagle's algorithm to fix 40ms ack delay in some case (.e.g send-send-40ms-recv)
     *
     * 40ms is the tcp ack delay on linux, which indicates that you are likely
     * encountering a bad interaction between delayed acks and the nagle's algorithm.
     *
     * the best way to address this is to send all of your data using a single call to
     * send() or sendmsg(), before waiting for a response.
     *
     * if that is not possible then certain tcp socket options including TCP_QUICKACK (on the receiving side),
     * TCP_CORK (sending side), and TCP_NODELAY (sending side) can help,
     * but can also hurt if used improperly.
     *
     * TCP_NODELAY simply disables the nagle's algorithm and is a one-time setting on the socket,
     * whereas the other two must be set at the appropriate times during the life of the connection
     * and can therefore be trickier to use.
     *
     * so we set TCP_NODELAY to reduce response delay for the accepted socket in the server by default
     */
    xu_int_t enable = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (xu_char_t*)&enable, sizeof(enable));

    // save address
    if (addr) xu_sockaddr_save(addr, &d);

    // ok
    return xu_fd2sock(fd);
}
xu_bool_t xu_socket_local(xu_socket_ref_t sock, xu_ipaddr_ref_t addr)
{
    // check
    xu_assert_and_check_return_val(sock, xu_false);

    // get local address
    struct sockaddr_storage d;
    xu_int_t                n = sizeof(d);
    if (getsockname(xu_sock2fd(sock), (struct sockaddr*)&d, (socklen_t*)&n) == -1) return xu_false;

    // save address
    if (addr) xu_sockaddr_save(addr, &d);

    // ok
    return xu_true;
}
xu_bool_t xu_socket_peer(xu_socket_ref_t sock, xu_ipaddr_ref_t addr)
{
    // check
    xu_assert_and_check_return_val(sock, xu_false);

    // get peer address
    struct sockaddr_storage d;
    xu_int_t                n = sizeof(d);
    if (getpeername(xu_sock2fd(sock), (struct sockaddr*)&d, (socklen_t*)&n) == -1) return xu_false;

    // save address
    if (addr) xu_sockaddr_save(addr, &d);

    // ok
    return xu_true;
}
xu_bool_t xu_socket_kill(xu_socket_ref_t sock, xu_size_t mode)
{
    // check
    xu_assert_and_check_return_val(sock, xu_false);

    // init how
    xu_int_t how = SHUT_RDWR;
    switch (mode)
    {
    case XU_SOCKET_KILL_RO: how = SHUT_RD; break;
    case XU_SOCKET_KILL_WO: how = SHUT_WR; break;
    case XU_SOCKET_KILL_RW: how = SHUT_RDWR; break;
    default: break;
    }

    // kill it
    xu_bool_t ok = !shutdown(xu_sock2fd(sock), how) ? xu_true : xu_false;

    // failed?
    if (!ok)
    {
        // trace
        xu_trace_e("kill: %p failed, errno: %d", sock, errno);
    }

    // ok?
    return ok;
}
xu_bool_t xu_socket_exit(xu_socket_ref_t sock)
{
    // check
    xu_assert_and_check_return_val(sock, xu_false);

    // trace
    xu_trace_d("close: %p", sock);

#ifdef XU_CONFIG_MODULE_HAVE_COROUTINE
    // attempt to cancel waiting from coroutine first
    xu_pointer_t       scheduler_io = xu_null;
    xu_poller_object_t object;
    object.type     = XU_POLLER_OBJECT_SOCK;
    object.ref.sock = sock;
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
    xu_bool_t ok = !close(xu_sock2fd(sock));

    // failed?
    if (!ok)
    {
        // trace
        xu_trace_e("close: %p failed, errno: %d", sock, errno);
    }

    // ok?
    return ok;
}
xu_long_t xu_socket_recv(xu_socket_ref_t sock, xu_byte_t* data, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(sock && data, -1);
    xu_check_return_val(size, 0);

    // recv
    xu_long_t real = recv(xu_sock2fd(sock), data, (xu_int_t)size, 0);

    // trace
    xu_trace_d("recv: %p %lu => %ld, errno: %d", sock, size, real, errno);

    // ok?
    if (real >= 0) return real;

    // continue?
    if (errno == EINTR || errno == EAGAIN) return 0;

    // error
    return -1;
}
xu_long_t xu_socket_send(xu_socket_ref_t sock, xu_byte_t const* data, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(sock && data, -1);
    xu_check_return_val(size, 0);

    // send
    xu_long_t real = send(xu_sock2fd(sock), data, (xu_int_t)size, 0);

    // trace
    xu_trace_d("send: %p %lu => %ld, errno: %d", sock, size, real, errno);

    // ok?
    if (real >= 0) return real;

    // continue?
    if (errno == EINTR || errno == EAGAIN) return 0;

    // error
    return -1;
}
xu_hong_t xu_socket_sendf(xu_socket_ref_t sock, xu_file_ref_t file, xu_hize_t offset, xu_hize_t size)
{
    // check
    xu_assert_and_check_return_val(sock && file && size, -1);

#if defined(XU_CONFIG_POSIX_HAVE_SENDFILE)

    // send it
    off_t     seek = offset;
    xu_hong_t real = sendfile(xu_sock2fd(sock), xu_sock2fd(file), &seek, (size_t)size);

    // ok?
    if (real >= 0) return real;

    // continue?
    if (errno == EINTR || errno == EAGAIN) return 0;

    // error
    return -1;

#elif defined(XU_CONFIG_OS_MACOSX) || defined(XU_CONFIG_OS_IOS)

    // send it
    off_t real = (off_t)size;
    if (!sendfile(xu_sock2fd(file), xu_sock2fd(sock), (off_t)offset, &real, xu_null, 0)) return (xu_hong_t)real;

    // continue?
    if (errno == EINTR || errno == EAGAIN) return (xu_hong_t)real;

    // error
    return -1;
#else

    // read data
    xu_byte_t data[8192];
    xu_long_t read = xu_file_pread(file, data, sizeof(data), offset);
    xu_check_return_val(read > 0, read);

    // send data
    xu_size_t writ = 0;
    while (writ < read)
    {
        xu_long_t real = xu_socket_send(sock, data + writ, read - writ);
        if (real > 0)
            writ += real;
        else
            break;
    }

    // ok?
    return writ == read ? writ : -1;
#endif
}
xu_long_t xu_socket_urecv(xu_socket_ref_t sock, xu_ipaddr_ref_t addr, xu_byte_t* data, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(sock && data, -1);

    // no size?
    xu_check_return_val(size, 0);

    // recv
    struct sockaddr_storage d;
    socklen_t               n = sizeof(d);
    xu_long_t               r = recvfrom(xu_sock2fd(sock), data, (xu_int_t)size, 0, (struct sockaddr*)&d, &n);

    // trace
    xu_trace_d("urecv: %p %lu bytes => %ld bytes, errno: %d", sock, size, r, errno);

    // ok?
    if (r >= 0)
    {
        // save address
        if (addr) xu_sockaddr_save(addr, &d);

        // ok
        return r;
    }

    // continue?
    if (errno == EINTR || errno == EAGAIN) return 0;

    // error
    return -1;
}
xu_long_t xu_socket_usend(xu_socket_ref_t sock, xu_ipaddr_ref_t addr, xu_byte_t const* data, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(sock && addr && data, -1);
    xu_assert_and_check_return_val(!xu_ipaddr_is_empty(addr), -1);

    // no size?
    xu_check_return_val(size, 0);

    // load addr
    xu_size_t               n = 0;
    struct sockaddr_storage d;
    if (!(n = xu_sockaddr_load(&d, addr))) return -1;

    // send
    xu_long_t r = sendto(xu_sock2fd(sock), data, (xu_int_t)size, 0, (struct sockaddr*)&d, n);

    // trace
    xu_trace_d("usend: %p %{ipaddr} %lu bytes => %ld bytes, errno: %d", sock, addr, size, r, errno);

    // ok?
    if (r >= 0) return r;

    // continue?
    if (errno == EINTR || errno == EAGAIN) return 0;

    // error
    return -1;
}
#ifndef XU_CONFIG_MICRO_ENABLE
xu_long_t xu_socket_recvv(xu_socket_ref_t sock, xu_iovec_t const* list, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(sock && list && size, -1);

    // check iovec
    xu_assert_static(sizeof(xu_iovec_t) == sizeof(struct iovec));
    xu_assert(xu_memberof_eq(xu_iovec_t, data, struct iovec, iov_base));
    xu_assert(xu_memberof_eq(xu_iovec_t, size, struct iovec, iov_len));

    // read it
    xu_long_t real = readv(xu_sock2fd(sock), (struct iovec const*)list, size);

    // ok?
    if (real >= 0) return real;

    // continue?
    if (errno == EINTR || errno == EAGAIN) return 0;

    // error
    return -1;
}
xu_long_t xu_socket_sendv(xu_socket_ref_t sock, xu_iovec_t const* list, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(sock && list && size, -1);

    // check iovec
    xu_assert_static(sizeof(xu_iovec_t) == sizeof(struct iovec));
    xu_assert(xu_memberof_eq(xu_iovec_t, data, struct iovec, iov_base));
    xu_assert(xu_memberof_eq(xu_iovec_t, size, struct iovec, iov_len));

    // writ it
    xu_long_t real = writev(xu_sock2fd(sock), (struct iovec const*)list, size);

    // ok?
    if (real >= 0) return real;

    // continue?
    if (errno == EINTR || errno == EAGAIN) return 0;

    // error
    return -1;
}
xu_long_t xu_socket_urecvv(xu_socket_ref_t sock, xu_ipaddr_ref_t addr, xu_iovec_t const* list, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(sock && list && size, -1);

    // init msg
    struct msghdr           msg = {0};
    struct sockaddr_storage d;
    msg.msg_name       = (xu_pointer_t)&d;
    msg.msg_namelen    = sizeof(d);
    msg.msg_iov        = (struct iovec*)list;
    msg.msg_iovlen     = (size_t)size;
    msg.msg_control    = xu_null;
    msg.msg_controllen = 0;
    msg.msg_flags      = 0;

    // recv
    xu_long_t r = recvmsg(xu_sock2fd(sock), &msg, 0);

    // ok?
    if (r >= 0)
    {
        // save address
        if (addr) xu_sockaddr_save(addr, &d);

        // ok
        return r;
    }

    // continue?
    if (errno == EINTR || errno == EAGAIN) return 0;

    // error
    return -1;
}
xu_long_t xu_socket_usendv(xu_socket_ref_t sock, xu_ipaddr_ref_t addr, xu_iovec_t const* list, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(sock && addr && list && size, -1);
    xu_assert_and_check_return_val(!xu_ipaddr_is_empty(addr), -1);

    // load addr
    xu_size_t               n = 0;
    struct sockaddr_storage d;
    if (!(n = xu_sockaddr_load(&d, addr))) return -1;

    // init msg
    struct msghdr msg  = {0};
    msg.msg_name       = (xu_pointer_t)&d;
    msg.msg_namelen    = n;
    msg.msg_iov        = (struct iovec*)list;
    msg.msg_iovlen     = (size_t)size;
    msg.msg_control    = xu_null;
    msg.msg_controllen = 0;
    msg.msg_flags      = 0;

    // recv
    xu_long_t r = sendmsg(xu_sock2fd(sock), &msg, 0);

    // ok?
    if (r >= 0) return r;

    // continue?
    if (errno == EINTR || errno == EAGAIN) return 0;

    // error
    return -1;
}
#endif
