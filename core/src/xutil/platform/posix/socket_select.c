#include "xutil/platform/socket.h"
#include <errno.h>
#include <sys/select.h>
#include <sys/socket.h>

/* *******************************************************
 * implementation
 */
xu_long_t xu_socket_wait_impl(xu_socket_ref_t sock, xu_size_t events, xu_long_t timeout)
{
    // check
    xu_assert_and_check_return_val(sock, -1);

    // fd
    xu_long_t fd = xu_sock2fd(sock);
    xu_assert_and_check_return_val(fd >= 0, -1);

    // init time
    struct timeval t = {0};
    if (timeout > 0)
    {
        t.tv_sec  = (timeout / 1000);
        t.tv_usec = (timeout % 1000) * 1000;
    }

    // init fds
    fd_set  rfds;
    fd_set  wfds;
    fd_set* prfds = (events & XU_SOCKET_EVENT_RECV) ? &rfds : xu_null;
    fd_set* pwfds = (events & XU_SOCKET_EVENT_SEND) ? &wfds : xu_null;

    if (prfds)
    {
        FD_ZERO(prfds);
        FD_SET(fd, prfds);
    }

    if (pwfds)
    {
        FD_ZERO(pwfds);
        FD_SET(fd, pwfds);
    }

    // select
    xu_long_t r = select(fd + 1, prfds, pwfds, xu_null, timeout >= 0 ? &t : xu_null);
    if (!r || (r == -1 && errno == EINTR)) // timeout or interrupted?
        return 0;
    xu_assert_and_check_return_val(r >= 0, -1);

    // check socket error?
    xu_int_t  error = 0;
    socklen_t n     = sizeof(socklen_t);
    if (!getsockopt(fd, SOL_SOCKET, SO_ERROR, (xu_char_t*)&error, &n) && error) return -1;

    // ok
    xu_long_t e = XU_SOCKET_EVENT_NONE;
    if (prfds && FD_ISSET(fd, &rfds)) e |= XU_SOCKET_EVENT_RECV;
    if (pwfds && FD_ISSET(fd, &wfds)) e |= XU_SOCKET_EVENT_SEND;
    return e;
}
