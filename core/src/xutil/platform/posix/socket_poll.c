#include "xutil/platform/socket.h"

#include <errno.h>
#include <poll.h>
#include <sys/socket.h>

/* *******************************************************
 * implementation
 */
xu_long_t xu_socket_wait_impl(xu_socket_ref_t sock, xu_size_t events, xu_long_t timeout)
{
    // check
    xu_assert_and_check_return_val(sock, -1);

    // init
    struct pollfd pfd = {0};
    pfd.fd            = xu_sock2fd(sock);
    if (events & XU_SOCKET_EVENT_RECV) pfd.events |= POLLIN;
    if (events & XU_SOCKET_EVENT_SEND) pfd.events |= POLLOUT;

    // poll
    xu_long_t r = poll(&pfd, 1, timeout);

    // timeout or interrupted?
    if (!r || (r == -1 && errno == EINTR)) return 0;

    // poll error?
    xu_assert_and_check_return_val(r >= 0, -1);

    // socket error?
    xu_int_t  o = 0;
    socklen_t n = sizeof(socklen_t);
    getsockopt(pfd.fd, SOL_SOCKET, SO_ERROR, &o, &n);
    if (o) return -1;

    // ok
    xu_long_t e = XU_SOCKET_EVENT_NONE;
    if (pfd.revents & POLLIN) e |= XU_SOCKET_EVENT_RECV;
    if (pfd.revents & POLLOUT) e |= XU_SOCKET_EVENT_SEND;
    if ((pfd.revents & POLLHUP) && !(e & (XU_SOCKET_EVENT_RECV | XU_SOCKET_EVENT_SEND)))
        e |= XU_SOCKET_EVENT_RECV | XU_SOCKET_EVENT_SEND;
    return e;
}
