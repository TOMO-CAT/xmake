/*!The Treasure Box Library
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2009-present, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        socket_select.c
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_long_t tb_socket_wait_impl(tb_socket_ref_t sock, tb_size_t events, tb_long_t timeout)
{
    // check
    tb_assert_and_check_return_val(sock, -1);

    // fd
    tb_long_t fd = tb_sock2fd(sock);
    tb_assert_and_check_return_val(fd >= 0, -1);

    // init time
    struct timeval t = {0};
    if (timeout > 0)
    {
        t.tv_sec = (timeout / 1000);
        t.tv_usec = (timeout % 1000) * 1000;
    }

    // init fds
    fd_set  rfds;
    fd_set  wfds;
    fd_set* prfds = (events & TB_SOCKET_EVENT_RECV)? &rfds : tb_null;
    fd_set* pwfds = (events & TB_SOCKET_EVENT_SEND)? &wfds : tb_null;

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
    tb_long_t r = select(fd + 1, prfds, pwfds, tb_null, timeout >= 0? &t : tb_null);
    if (!r || (r == -1 && errno == EINTR)) // timeout or interrupted?
        return 0;
    tb_assert_and_check_return_val(r >= 0, -1);

    // check socket error?
    tb_int_t error = 0;
    socklen_t n = sizeof(socklen_t);
    if (!getsockopt(fd, SOL_SOCKET, SO_ERROR, (tb_char_t*)&error, &n) && error)
        return -1;

    // ok
    tb_long_t e = TB_SOCKET_EVENT_NONE;
    if (prfds && FD_ISSET(fd, &rfds)) e |= TB_SOCKET_EVENT_RECV;
    if (pwfds && FD_ISSET(fd, &wfds)) e |= TB_SOCKET_EVENT_SEND;
    return e;
}
