/* *******************************************************
 * trace
 * *******************************************************
 */
#define XU_TRACE_MODULE_NAME "stream_sock"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/platform/addrinfo.h"
#include "xutil/stream/impl/stream.h"
#include "xutil/utils/state.h"

/* *******************************************************
 * macros
 * *******************************************************
 */

// the sock cache maxn
#ifdef __xu_small__
#    define XU_STREAM_SOCK_CACHE_MAXN (8192)
#else
#    define XU_STREAM_SOCK_CACHE_MAXN (8192 << 1)
#endif

/* *******************************************************
 * types
 * *******************************************************
 */

// the sock stream type
typedef struct __xu_stream_sock_t
{
    // the sock
    xu_socket_ref_t sock;

    // the sock type
    xu_uint32_t type : 22;

    // the try number
    xu_uint32_t tryn : 8;

    // keep alive after being closed?
    xu_uint32_t keep_alive : 1;

    // is owner of socket
    xu_uint32_t owner : 1;

    // the wait event
    xu_long_t wait;

    // the read size
    xu_size_t read;

    // the writ size
    xu_size_t writ;

} xu_stream_sock_t;

/* *******************************************************
 * implementation
 * *******************************************************
 */

static __xu_inline__ xu_stream_sock_t* xu_stream_sock_cast(xu_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && xu_stream_type(stream) == XU_STREAM_TYPE_SOCK, xu_null);

    // ok?
    return (xu_stream_sock_t*)stream;
}
static xu_bool_t xu_stream_sock_open(xu_stream_ref_t stream)
{
    // check
    xu_stream_sock_t* stream_sock = xu_stream_sock_cast(stream);
    xu_assert_and_check_return_val(stream_sock && stream_sock->type, xu_false);

    // clear
    stream_sock->wait = 0;
    stream_sock->tryn = 0;
    stream_sock->read = 0;
    stream_sock->writ = 0;

    // the url
    xu_url_ref_t url = xu_stream_url(stream);
    xu_assert_and_check_return_val(url, xu_false);

    // ssl? not supported
    if (xu_url_ssl(url))
    {
        // trace
        xu_trace_w("ssl is not supported now! please enable it from config if you need it.");

        // save state
        xu_stream_state_set(stream, XU_STATE_SOCK_SSL_NOT_SUPPORTED);
        return xu_false;
    }

    // get address from the url
    xu_ipaddr_ref_t addr = xu_url_addr(url);
    xu_assert_and_check_return_val(addr, xu_false);

    // get the port
    xu_uint16_t port = xu_ipaddr_port(addr);
    xu_assert_and_check_return_val(port, xu_false);

    // no ip?
    if (xu_ipaddr_ip_is_empty(addr))
    {
        // look ip
        xu_ipaddr_t ip_addr = {0};
        if (!xu_addrinfo_addr(xu_url_host(url), &ip_addr))
        {
            xu_stream_state_set(stream, XU_STATE_SOCK_DNS_FAILED);
            return xu_false;
        }

        // update address to the url
        xu_ipaddr_ip_set(addr, &ip_addr);
    }

    // tcp or udp? for url: sock://ip:port/?udp=
    xu_char_t const* args = xu_url_args(url);
    if (args && !xu_strnicmp(args, "udp=", 4))
        stream_sock->type = XU_SOCKET_TYPE_UDP;
    else if (args && !xu_strnicmp(args, "tcp=", 4))
        stream_sock->type = XU_SOCKET_TYPE_TCP;

    // exit sock first if not keep-alive
    if (!stream_sock->keep_alive && stream_sock->sock)
    {
        if (stream_sock->sock && !xu_socket_exit(stream_sock->sock)) return xu_false;
        stream_sock->sock = xu_null;
    }

    // make sock
    if (!stream_sock->sock) stream_sock->sock = xu_socket_init(stream_sock->type, xu_ipaddr_family(addr));

    // open sock failed?
    if (!stream_sock->sock)
    {
        // trace
        xu_trace_e("open sock failed!");

        // save state
        xu_stream_state_set(stream, XU_STATE_SOCK_OPEN_FAILED);
        return xu_false;
    }

    // done
    xu_bool_t ok = xu_false;
    switch (stream_sock->type)
    {
    case XU_SOCKET_TYPE_TCP:
    {
        // trace
        xu_trace_d("sock(%p): connect: %s[%{ipaddr}]: ..", stream_sock->sock, xu_url_host(url), addr);

        // connect it
        xu_long_t real = -1;
        while (!(real = xu_socket_connect(stream_sock->sock, addr)) && !xu_stream_is_killed(stream))
        {
            // wait it
            real = xu_socket_wait(stream_sock->sock, XU_SOCKET_EVENT_CONN, xu_stream_timeout(stream));
            xu_check_break(real > 0);
        }

        // ok?
        if (real > 0)
        {
            ok = xu_true;
            xu_stream_state_set(stream, XU_STATE_OK);
        }
        else
            xu_stream_state_set(stream, !real ? XU_STATE_SOCK_CONNECT_TIMEOUT : XU_STATE_SOCK_CONNECT_FAILED);

        // trace
        xu_trace_d("sock(%p): connect: %s", stream_sock->sock, ok ? "ok" : "failed");

        // ok?
        if (ok)
        {
            // ssl? init it
            xu_url_ssl(url);
        }
    }
    break;
    case XU_SOCKET_TYPE_UDP:
    {
        // ssl? not supported
        if (xu_url_ssl(url))
        {
            // trace
            xu_trace_w("udp ssl is not supported!");

            // save state
            xu_stream_state_set(stream, XU_STATE_SOCK_SSL_NOT_SUPPORTED);
        }
        else
        {
            // ok
            ok = xu_true;
            xu_stream_state_set(stream, XU_STATE_OK);
        }
    }
    break;
    default:
    {
        // trace
        xu_trace_e("unknown socket type: %lu", stream_sock->type);
    }
    break;
    }

    // open failed? close ssl and socket
    if (!ok)
    {
        // exit sock
        if (stream_sock->sock) xu_socket_exit(stream_sock->sock);
        stream_sock->sock = xu_null;
    }

    // ok?
    return ok;
}
static xu_bool_t xu_stream_sock_open_ref(xu_stream_ref_t stream)
{
    // check
    xu_stream_sock_t* stream_sock = xu_stream_sock_cast(stream);
    xu_assert_and_check_return_val(stream_sock && stream_sock->type, xu_false);

    // clear
    stream_sock->wait = 0;
    stream_sock->tryn = 0;
    stream_sock->read = 0;
    stream_sock->writ = 0;

    // the url
    xu_url_ref_t url = xu_stream_url(stream);
    xu_assert_and_check_return_val(url, xu_false);

    // ssl? not supported
    if (xu_url_ssl(url))
    {
        // trace
        xu_trace_w("ssl is not supported now! please enable it from config if you need it.");

        // save state
        xu_stream_state_set(stream, XU_STATE_SOCK_SSL_NOT_SUPPORTED);
        return xu_false;
    }

    // done
    xu_bool_t ok = xu_false;
    switch (stream_sock->type)
    {
    case XU_SOCKET_TYPE_TCP:
    {
        // ssl? init it
        if (xu_url_ssl(url))
        {
        }
        else
            ok = xu_true;
    }
    break;
    case XU_SOCKET_TYPE_UDP:
    {
        // ssl? not supported
        if (xu_url_ssl(url))
        {
            // trace
            xu_trace_w("udp ssl is not supported!");

            // save state
            xu_stream_state_set(stream, XU_STATE_SOCK_SSL_NOT_SUPPORTED);
        }
        else
        {
            // ok
            ok = xu_true;
            xu_stream_state_set(stream, XU_STATE_OK);
        }
    }
    break;
    default:
    {
        // trace
        xu_trace_e("unknown socket type: %lu", stream_sock->type);
    }
    break;
    }

    // open failed? close ssl and socket
    if (!ok)
    {
    }

    // ok?
    return ok;
}
static xu_bool_t xu_stream_sock_clos(xu_stream_ref_t stream)
{
    // check
    xu_stream_sock_t* stream_sock = xu_stream_sock_cast(stream);
    xu_assert_and_check_return_val(stream_sock, xu_false);

    // keep alive? not close it
    xu_check_return_val(!stream_sock->keep_alive, xu_true);

    // exit socket
    if (stream_sock->owner)
    {
        if (stream_sock->sock && !xu_socket_exit(stream_sock->sock)) return xu_false;
        stream_sock->sock = xu_null;
    }

    // clear
    stream_sock->wait = 0;
    stream_sock->tryn = 0;
    stream_sock->read = 0;
    stream_sock->writ = 0;

    // ok
    return xu_true;
}
static xu_void_t xu_stream_sock_exit(xu_stream_ref_t stream)
{
    // check
    xu_stream_sock_t* stream_sock = xu_stream_sock_cast(stream);
    xu_assert_and_check_return(stream_sock);

    // exit sock
    if (stream_sock->sock && stream_sock->owner) xu_socket_exit(stream_sock->sock);
    stream_sock->sock = xu_null;

    // clear
    stream_sock->wait = 0;
    stream_sock->tryn = 0;
    stream_sock->read = 0;
    stream_sock->writ = 0;
}
static xu_void_t xu_stream_sock_kill(xu_stream_ref_t stream)
{
    // check
    xu_stream_sock_t* stream_sock = xu_stream_sock_cast(stream);
    xu_assert_and_check_return(stream_sock);

    // kill it
    if (stream_sock->sock) xu_socket_kill(stream_sock->sock, XU_SOCKET_KILL_RW);
}
static xu_long_t xu_stream_sock_read(xu_stream_ref_t stream, xu_byte_t* data, xu_size_t size)
{
    // check
    xu_stream_sock_t* stream_sock = xu_stream_sock_cast(stream);
    xu_assert_and_check_return_val(stream_sock && stream_sock->sock, -1);

    // the url
    xu_url_ref_t url = xu_stream_url(stream);
    xu_assert_and_check_return_val(url, -1);

    // check
    xu_check_return_val(data, -1);
    xu_check_return_val(size, 0);

    // clear writ
    stream_sock->writ = 0;

    // read
    xu_long_t real = -1;
    switch (stream_sock->type)
    {
    case XU_SOCKET_TYPE_TCP:
    {
        {
            // read data
            real = xu_socket_recv(stream_sock->sock, data, size);

            // trace
            xu_trace_d("sock(%p): read: %ld <? %lu", stream_sock->sock, real, size);

            // failed or closed?
            xu_check_return_val(real >= 0, -1);

            // peer closed?
            if (!real && stream_sock->wait > 0 && (stream_sock->wait & XU_SOCKET_EVENT_RECV)) return -1;

            // clear wait
            if (real > 0) stream_sock->wait = 0;
        }
    }
    break;
    case XU_SOCKET_TYPE_UDP:
    {
        // read data
        real = xu_socket_urecv(stream_sock->sock, xu_null, data, size);

        // trace
        xu_trace_d("sock(%p): read: %ld <? %lu", stream_sock->sock, real, size);

        // failed or closed?
        xu_check_return_val(real >= 0, -1);

        // peer closed?
        if (!real && stream_sock->wait > 0 && (stream_sock->wait & XU_SOCKET_EVENT_RECV)) return -1;

        // clear wait
        if (real > 0) stream_sock->wait = 0;
    }
    break;
    default: break;
    }

    // update read
    if (real > 0) stream_sock->read += real;

    // ok?
    return real;
}
static xu_long_t xu_stream_sock_writ(xu_stream_ref_t stream, xu_byte_t const* data, xu_size_t size)
{
    // check
    xu_stream_sock_t* stream_sock = xu_stream_sock_cast(stream);
    xu_assert_and_check_return_val(stream_sock && stream_sock->sock, -1);

    // the url
    xu_url_ref_t url = xu_stream_url(stream);
    xu_assert_and_check_return_val(url, -1);

    // check
    xu_check_return_val(data, -1);
    xu_check_return_val(size, 0);

    // clear read
    stream_sock->read = 0;

    // writ
    xu_long_t real = -1;
    switch (stream_sock->type)
    {
    case XU_SOCKET_TYPE_TCP:
    {
        {
            // writ data
            real = xu_socket_send(stream_sock->sock, data, size);

            // trace
            xu_trace_d("sock(%p): writ: %ld <? %lu", stream_sock->sock, real, size);

            // failed or closed?
            xu_check_return_val(real >= 0, -1);

            // peer closed?
            if (!real && stream_sock->wait > 0 && (stream_sock->wait & XU_SOCKET_EVENT_SEND)) return -1;

            // clear wait
            if (real > 0) stream_sock->wait = 0;
        }
    }
    break;
    case XU_SOCKET_TYPE_UDP:
    {
        // get address from the url
        xu_ipaddr_ref_t addr = xu_url_addr(url);
        xu_assert_and_check_return_val(addr, -1);

        // writ data
        real = xu_socket_usend(stream_sock->sock, addr, data, size);

        // trace
        xu_trace_d("sock(%p): writ: %ld <? %lu", stream_sock->sock, real, size);

        // failed or closed?
        xu_check_return_val(real >= 0, -1);

        // no data?
        if (!real)
        {
            // abort? writ x, writ 0, or writ 0, writ 0
            xu_check_return_val(!stream_sock->writ && !stream_sock->tryn, -1);

            // tryn++
            stream_sock->tryn++;
        }
        else
            stream_sock->tryn = 0;
    }
    break;
    default: break;
    }

    // update writ
    if (real > 0) stream_sock->writ += real;

    // ok?
    return real;
}
static xu_long_t xu_stream_sock_wait(xu_stream_ref_t stream, xu_size_t wait, xu_long_t timeout)
{
    // check
    xu_stream_sock_t* stream_sock = xu_stream_sock_cast(stream);
    xu_assert_and_check_return_val(stream_sock && stream_sock->sock, -1);

    {
        // wait
        stream_sock->wait = xu_socket_wait(stream_sock->sock, wait, timeout);
    }

    // trace
    xu_trace_d("sock(%p): wait: %ld", stream_sock->sock, stream_sock->wait);

    // ok?
    return stream_sock->wait;
}
static xu_bool_t xu_stream_sock_ctrl(xu_stream_ref_t stream, xu_size_t ctrl, xu_va_list_t args)
{
    xu_stream_sock_t* stream_sock = xu_stream_sock_cast(stream);
    xu_assert_and_check_return_val(stream_sock, xu_false);

    switch (ctrl)
    {
    case XU_STREAM_CTRL_SOCK_SET_TYPE:
    {
        // check
        xu_assert_and_check_return_val(xu_stream_is_closed(stream), xu_false);
        xu_assert_and_check_return_val(!stream_sock->keep_alive, xu_false);

        // the type
        xu_size_t type = (xu_size_t)xu_va_arg(args, xu_size_t);
        xu_assert_and_check_return_val(type == XU_SOCKET_TYPE_TCP || type == XU_SOCKET_TYPE_UDP, xu_false);

        // set type
        stream_sock->type = type;

        // ok
        return xu_true;
    }
    case XU_STREAM_CTRL_SOCK_GET_TYPE:
    {
        xu_size_t* ptype = (xu_size_t*)xu_va_arg(args, xu_size_t*);
        xu_assert_and_check_return_val(ptype, xu_false);
        *ptype = stream_sock->type;
        return xu_true;
    }
    case XU_STREAM_CTRL_SOCK_KEEP_ALIVE:
    {
        // keep alive?
        xu_bool_t keep_alive    = (xu_bool_t)xu_va_arg(args, xu_bool_t);
        stream_sock->keep_alive = keep_alive ? 1 : 0;
        return xu_true;
    }
    case XU_STREAM_CTRL_SOCK_GET_SOCK:
    {
        // the psock
        xu_socket_ref_t* psock = (xu_socket_ref_t*)xu_va_arg(args, xu_socket_ref_t*);
        xu_assert_and_check_return_val(psock, xu_false);

        // get sock
        *psock = stream_sock->sock;
        return xu_true;
    }
    default: break;
    }
    return xu_false;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
xu_stream_ref_t xu_stream_init_sock()
{
    // init stream
    xu_stream_ref_t stream =
        xu_stream_init(XU_STREAM_TYPE_SOCK, sizeof(xu_stream_sock_t), XU_STREAM_SOCK_CACHE_MAXN, xu_stream_sock_open,
                       xu_stream_sock_clos, xu_stream_sock_exit, xu_stream_sock_ctrl, xu_stream_sock_wait,
                       xu_stream_sock_read, xu_stream_sock_writ, xu_null, xu_null, xu_stream_sock_kill);
    xu_assert_and_check_return_val(stream, xu_null);

    // init the sock stream
    xu_stream_sock_t* stream_sock = xu_stream_sock_cast(stream);
    if (stream_sock)
    {
        // init sock type
        stream_sock->type = XU_SOCKET_TYPE_TCP;

        // mark as owner of socket
        stream_sock->owner = 1;
    }

    // ok?
    return stream;
}
xu_stream_ref_t xu_stream_init_from_sock(xu_char_t const* host, xu_uint16_t port, xu_size_t type, xu_bool_t bssl)
{
    // check
    xu_assert_and_check_return_val(host && port, xu_null);

    // done
    xu_bool_t       ok     = xu_false;
    xu_stream_ref_t stream = xu_null;
    do
    {
        // init stream
        stream = xu_stream_init_sock();
        xu_assert_and_check_break(stream);

        // ctrl stream
        if (!xu_stream_ctrl(stream, XU_STREAM_CTRL_SET_HOST, host)) break;
        if (!xu_stream_ctrl(stream, XU_STREAM_CTRL_SET_PORT, port)) break;
        if (!xu_stream_ctrl(stream, XU_STREAM_CTRL_SET_SSL, bssl)) break;
        if (!xu_stream_ctrl(stream, XU_STREAM_CTRL_SOCK_SET_TYPE, type)) break;

        // init the sock stream
        xu_stream_sock_t* stream_sock = xu_stream_sock_cast(stream);
        xu_assert_and_check_break(stream_sock);

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (stream) xu_stream_exit(stream);
        stream = xu_null;
    }

    // ok?
    return stream;
}
xu_stream_ref_t xu_stream_init_from_sock_ref(xu_socket_ref_t sock, xu_size_t type, xu_bool_t bssl)
{
    // check
    xu_assert_and_check_return_val(sock, xu_null);

    // done
    xu_bool_t       ok     = xu_false;
    xu_stream_ref_t stream = xu_null;
    do
    {
        // init stream
        stream = xu_stream_init(XU_STREAM_TYPE_SOCK, sizeof(xu_stream_sock_t), XU_STREAM_SOCK_CACHE_MAXN,
                                xu_stream_sock_open_ref, xu_stream_sock_clos, xu_stream_sock_exit, xu_stream_sock_ctrl,
                                xu_stream_sock_wait, xu_stream_sock_read, xu_stream_sock_writ, xu_null, xu_null,
                                xu_stream_sock_kill);
        xu_assert_and_check_break(stream);

        // ctrl stream
        if (!xu_stream_ctrl(stream, XU_STREAM_CTRL_SET_HOST, "fd")) break;
        if (!xu_stream_ctrl(stream, XU_STREAM_CTRL_SET_PORT, (xu_uint16_t)xu_sock2fd(sock))) break;
        if (!xu_stream_ctrl(stream, XU_STREAM_CTRL_SET_SSL, bssl)) break;
        if (!xu_stream_ctrl(stream, XU_STREAM_CTRL_SOCK_SET_TYPE, type)) break;

        // init the sock stream
        xu_stream_sock_t* stream_sock = xu_stream_sock_cast(stream);
        xu_assert_and_check_break(stream_sock);

        // only be reference of socket
        stream_sock->owner = 0;

        // save socket
        stream_sock->sock = sock;

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (stream) xu_stream_exit(stream);
        stream = xu_null;
    }

    // ok?
    return stream;
}
